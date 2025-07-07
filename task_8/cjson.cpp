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
        std::cout << "ERROR: Exception occurred during value casting " << ex.what() << std::endl;
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
        std::cout << "ERROR: Invalid string format (invalid delimiter)" << std::endl;
        return {NULL, NULL};
    }

    std::string_view key_str = parse_Quotes(std::string_view(kv_str.begin(), delimiter_it));
    std::cout << "key_str = " << key_str << std::endl;
    int64_t posible_val = 0;
    try
    {
        if (is_Integer(std::string(std::next(delimiter_it), kv_str.end()), posible_val))
        {
            std::cout << "posible_val = " << posible_val << std::endl;
            if (!(value = Py_BuildValue("i", posible_val)))
            {
                std::cout << "ERROR: Failed to build integer value" << std::endl;
                return {NULL, NULL};
            }
        }
        else
        {
            std::string_view value_str = parse_Quotes(std::string_view(std::next(delimiter_it), kv_str.end()));
            std::cout << "value_str = " << value_str << std::endl;
            if (!(value = Py_BuildValue("s", value_str)))
            {
                std::cout << "ERROR: Failed to build string value" << std::endl;
                return {NULL, NULL};
            }
        }
    }
    catch (std::exception &ex)
    {
        return {NULL, NULL};
    }

    if (!(key = Py_BuildValue("s", key_str)))
    {
        std::cout << "ERROR: Failed to build string value" << std::endl;
        return {NULL, NULL};
    }

    return {key, value};
}

static PyObject *loads(PyObject *self, PyObject *args)
{
    PyObject *dict = NULL;
    if (!(dict = PyDict_New()))
    {
        std::cout << "ERROR: Failed to create Dict Object" << std::endl;
        return NULL;
    }

    const char *json_dump;
    if (!PyArg_ParseTuple(args, "s", &json_dump))
        return NULL;

    std::string dump_s(json_dump);

    if (!dump_s.empty() && (dump_s.back() != '}' || dump_s.front() != '{'))
    {
        std::cout << "ERROR: Incorrect json serialization (must be wrapped in curly braces)" << std::endl;
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
                std::cout << "ERROR: Failed to set item" << std::endl;
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
        std::cout << "ERROR: Failed to set item" << std::endl;
        return NULL;
    }
    return dict;
}

int main()
{
    // std::string block = "\"hello\": 10";
    // std::string block1 = "\"world\": \"value\"";

    // auto p = build_KeyValue_Object(block);
    // p = build_KeyValue_Object(block1);
}