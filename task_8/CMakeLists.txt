cmake_minimum_required(VERSION 3.12)
project(Cjson_Module)

find_package(Python REQUIRED COMPONENTS Interpreter Development)

execute_process(
    COMMAND ${Python_EXECUTABLE} -c "import sys; print(sys.prefix != sys.base_prefix)"
    OUTPUT_VARIABLE IN_VENV
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT IN_VENV)
    set(VENV_DIR "${CMAKE_BINARY_DIR}/venv")
    message(STATUS "Creating virtual environment in ${VENV_DIR}")

    execute_process(
        COMMAND ${Python_EXECUTABLE} -m venv ${VENV_DIR}
        RESULT_VARIABLE VENV_RESULT
    )

    if(NOT VENV_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to create virtual environment")
    endif()

    set(Python_EXECUTABLE "${VENV_DIR}/bin/python")
endif()

execute_process(
    COMMAND ${Python_EXECUTABLE} -c "import site; print(site.getsitepackages()[0])"
    OUTPUT_VARIABLE PYTHON_SITEPACKAGES
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(MODULE_NAME "cjson_module")
set(SETUP_PY "${CMAKE_CURRENT_SOURCE_DIR}/setup.py")

add_custom_command(
    OUTPUT ${PYTHON_SITEPACKAGES}/${MODULE_NAME}
    COMMAND ${Python_EXECUTABLE} ${SETUP_PY} build_ext --inplace
    COMMAND ${Python_EXECUTABLE} ${SETUP_PY} install --user
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Installing Python module with setup.py"
)

add_custom_target(
    install_module
    DEPENDS ${PYTHON_SITEPACKAGES}/${MODULE_NAME}
)


option(BUILD_TESTING "Build tests" ON)

if(BUILD_TESTING)
    execute_process(
        COMMAND ${Python_EXECUTABLE} -c "import pytest"
        RESULT_VARIABLE PYTEST_FOUND
    )

    if(NOT PYTEST_FOUND EQUAL 0)
        message(STATUS "Installing pytest in virtual environment")
        execute_process(
            COMMAND ${Python_EXECUTABLE} -m pip install pytest
            RESULT_VARIABLE PYTEST_INSTALL_RESULT
        )

        if(NOT PYTEST_INSTALL_RESULT EQUAL 0)
            message(WARNING "Failed to install pytest, tests will be disabled")
            set(PYTEST_FOUND 1)
        else()
            set(PYTEST_FOUND 0)
        endif()
    endif()

    if(PYTEST_FOUND EQUAL 0)
        add_test(
            NAME module_test
            COMMAND ${Python_EXECUTABLE} -m pytest tests/
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )

        add_custom_target(
            test
            COMMAND ${Python_EXECUTABLE} -m pytest -v tests/
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS install_module
        )
    else()
        message(WARNING "pytest not found, tests will be disabled")
    endif()

    option(BUILD_BENCHMARKS "Build benchmarks" ON)

    if(BUILD_BENCHMARKS)

        execute_process(
            COMMAND ${Python_EXECUTABLE} -c "import ujson"
            RESULT_VARIABLE UJSON_FOUND
        )

        if(NOT UJSON_FOUND EQUAL 0)
            message(STATUS "Installing ujson for benchmarks")
            execute_process(
                COMMAND ${Python_EXECUTABLE} -m pip install ujson
                RESULT_VARIABLE UJSON_INSTALL_RESULT
            )

            if(NOT UJSON_INSTALL_RESULT EQUAL 0)
                message(WARNING "Failed to install ujson, benchmarks will be disabled")
                set(UJSON_FOUND 1)
            else()
                set(UJSON_FOUND 0)
            endif()
        endif()

        if(NOT FAKER_FOUND EQUAL 0)
            message(STATUS "Installing Faker for test data generation")
            execute_process(
                COMMAND ${Python_EXECUTABLE} -m pip install faker
                RESULT_VARIABLE FAKER_INSTALL_RESULT
            )

            if(NOT FAKER_INSTALL_RESULT EQUAL 0)
                message(WARNING "Failed to install Faker, will use simple random data")
                set(FAKER_FOUND 1)
            else()
                set(FAKER_FOUND 0)
            endif()
        endif()

        if(UJSON_FOUND EQUAL 0)
            file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dataset)

            add_custom_target(
                benchmark
                COMMAND ${Python_EXECUTABLE} -m pytest tests/benchmark.py -v -s
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                DEPENDS install_module
                COMMENT "Running performance benchmarks"
            )

            # add_custom_target(
            #     generate_data
            #     COMMAND ${Python_EXECUTABLE} -c
            #     "import sys; sys.path.append('${CMAKE_CURRENT_SOURCE_DIR}/tests');
            #     from generate_data import generate_test_files;
            #     generate_test_files('${CMAKE_CURRENT_SOURCE_DIR}/dataset', 
            #     num_entries=1000000, num_files=5)"
            #     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            #     COMMENT "Generating test data files in dataset directory"
            # )
        endif()
    endif()

endif()
