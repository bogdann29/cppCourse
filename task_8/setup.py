from setuptools import Extension, setup


def main():
    setup(
        name="cjson",
        version="0.0.1",
        description="cjson module",
        author="Pustov B.S.",
        ext_modules=[
            Extension("cjson", ["cjson.cpp"], extra_compile_args=["-std=c++20"])
        ],
    )


if __name__ == "__main__":
    main()
