#include <Python.h>
#include <map>
#include <string>
#include <algorithm>
#include <string_view>
#include <iterator>
#include <exception>
#include <iostream>

bool is_Integer(const std::string &s, int64_t &target)
{
    auto it = s.rbegin();
    for (; it != s.rend(); ++it)
        if (!(*it == ' ' || *it == '\t' || *it == '\n'))
            break;

    auto ln = static_cast<size_t>(s.rend() - it);
    try
    {
        size_t pos = 0;
        auto tmp = static_cast<int64_t>(std::stoll(s, &pos));
        if (pos != ln)
            return false;
        target = tmp;
    }
    catch (const std::invalid_argument &ex)
    {
        return false;
    }
    catch (const std::exception &ex)
    {
        throw ex;
    }

    return true;
}

std::string_view parse_Quotes(std::string_view str)
{
    auto is_quote = [](char c)
    { return c == '\"'; };

    auto first_quote = std::find_if(str.begin(), str.end(), is_quote);
    if (first_quote == str.end())
    {
        return {};
    }

    auto second_quote = std::find_if(first_quote + 1, str.end(), is_quote);
    if (second_quote == str.end())
    {
        return {};
    }

    return std::string_view(std::next(first_quote), second_quote);
}

// ' "world": "value" '  possible string
std::pair<PyObject *, PyObject *> build_KeyValue_Object(std::string_view kv_str)
{
    PyObject *key, *value;

    auto delimiter_it = std::find(kv_str.begin(), kv_str.end(), ':');
    if (delimiter_it == kv_str.end())
    {
        PyErr_Format(PyExc_ValueError, "ERROR: Invalid string format (invalid delimiter)");
        return {NULL, NULL};
    }

    std::string_view key_str = parse_Quotes(std::string_view(kv_str.begin(), delimiter_it));
    int64_t posible_val = 0;
    try
    {
        if (is_Integer(std::string(std::next(delimiter_it), kv_str.end()), posible_val))
        {
            if (!(value = Py_BuildValue("i", posible_val)))
            {
                PyErr_Format(PyExc_ValueError, "ERROR: Failed to build integer value");
                return {NULL, NULL};
            }
        }
        else
        {
            std::string_view value_str = parse_Quotes(std::string_view(std::next(delimiter_it), kv_str.end()));
            if (!(value = Py_BuildValue("s", std::string(value_str).c_str())))
            {
                PyErr_Format(PyExc_ValueError, "ERROR: Failed to build string value");
                return {NULL, NULL};
            }
        }
    }
    catch (std::exception &ex)
    {
        PyErr_Format(PyExc_ValueError, "ERROR: Exception occurred during value casting %s", ex.what());
        return {NULL, NULL};
    }

    if (!(key = Py_BuildValue("s", std::string(key_str).c_str())))
    {
        PyErr_Format(PyExc_ValueError, "ERROR: Failed to build string value");
        return {NULL, NULL};
    }

    return {key, value};
}

static PyObject *loads(PyObject *self, PyObject *args)
{
    PyObject *dict = NULL;
    if (!(dict = PyDict_New()))
    {
        PyErr_Format(PyExc_TypeError, "ERROR: Failed to create Dict Object");
        return NULL;
    }

    const char *json_dump;
    if (!PyArg_ParseTuple(args, "s", &json_dump))
        return NULL;

    std::string dump_s(json_dump);

    if (!dump_s.empty() && (dump_s.back() != '}' || dump_s.front() != '{'))
    {
        PyErr_Format(PyExc_ValueError, "ERROR: Incorrect json serialization (must be wrapped in curly braces)");
        return NULL;
    }

    // {"hello": 10, "world": "value"}
    auto it = std::next(dump_s.begin());
    auto kv_block_start = it;
    bool is_str = false;
    while (it != std::prev(dump_s.end()))
    {
        if (*it == '\"')
            is_str ^= true;
        else if (!is_str && *it == ',')
        {
            auto [key, value] = build_KeyValue_Object(std::string_view(kv_block_start, it));
            if (key == NULL || value == NULL)
                return NULL;
            if (PyDict_SetItem(dict, key, value) < 0)
            {
                PyErr_Format(PyExc_TypeError, "ERROR: Failed to set item");
                return NULL;
            }
            kv_block_start = std::next(it);
        }
        ++it;
    }

    auto [key, value] = build_KeyValue_Object(std::string_view(kv_block_start, it));
    if (key == NULL && value == NULL)
        return NULL;
    if (PyDict_SetItem(dict, key, value) < 0)
    {
        PyErr_Format(PyExc_TypeError, "ERROR: Failed to set item");
        return NULL;
    }
    return dict;
}

static PyObject *dumps(PyObject *self, PyObject *args)
{
    PyObject *res = NULL;
    PyObject *json_dict;

    if (!PyArg_ParseTuple(args, "O", &json_dict))
        return NULL;

    if (!PyDict_Check(json_dict))
    {
        PyErr_Format(PyExc_TypeError, "ERROR: Argument must be a dictionary");
        return NULL;
    }

    std::string res_str = "{";
    PyObject *key, *value;
    Py_ssize_t pos = 0;
    std::string value_str;
    bool first_item = true;

    while (PyDict_Next(json_dict, &pos, &key, &value))
    {
        PyObject *key_pystr = PyObject_Str(key);
        if (!key_pystr)
        {
            PyErr_Format(PyExc_TypeError, "Key must be convertible to string");
            return NULL;
        }
        PyObject *value_pystr = PyObject_Str(value);
        if (!value_pystr)
        {
            PyErr_Format(PyExc_TypeError, "Value must be convertible to string");
            return NULL;
        }

        const char *key_str = PyUnicode_AsUTF8(key_pystr);
        const char *value_str = PyUnicode_AsUTF8(value_pystr);

        if (!first_item)
        {
            res_str += ", ";
        }

        if (PyLong_Check(value))
            res_str += "\"" + std::string(key_str) + "\": " + std::string(value_str);
        else
            res_str += "\"" + std::string(key_str) + "\": \"" + std::string(value_str) + "\"";

        first_item = false;
    }
    res_str += "}";

    if (!(res = Py_BuildValue("s", res_str.c_str())))
    {
        PyErr_Format(PyExc_ValueError, "ERROR: Failed to build string value");
        return NULL;
    }
    return res;
}

static PyMethodDef cjsonMethods[] = {
    {"loads", loads, METH_VARARGS, "Формирование словаря по строке в формате json"},
    {"dumps", dumps, METH_VARARGS, "Сериализация json словаря в строку"},
    {NULL, NULL, 0, NULL}};

static PyModuleDef cjsonModuleDef = {
    PyModuleDef_HEAD_INIT,
    "cjson",
    "Модуль cjson",
    -1,
    cjsonMethods};

PyMODINIT_FUNC PyInit_cjson(void)
{
    return PyModule_Create(&cjsonModuleDef);
}