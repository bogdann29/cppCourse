import pytest
import cjson

from pytest import raises

BASIC_TEST_CASES = [
    # Простые случаи
    ('{"name": "Bogdan"}', {"name": "Bogdan"}),
    ('{"age": 23}', {"age": 23}),
    # Несколько пар
    ('{"name": "Alice", "age": 9}', {"name": "Alice", "age": 9}),
    ('{"title": "Teacher", "salary": 5000}', {"title": "Teacher", "salary": 5000}),
    # Граничные значения чисел
    ('{"max_int": 2147483647}', {"max_int": 2147483647}),
    ('{"min_int": -2147483648}', {"min_int": -2147483648}),
    # 64-битные числа
    ('{"max_int64": 9223372036854775807}', {"max_int64": 9223372036854775807}),
    ('{"min_int64": -9223372036854775808}', {"min_int64": -9223372036854775808}),
    # Большие числа (>2^53) с проверкой точности
    ('{"big_num": 9007199254740993}', {"big_num": 9007199254740993}),
]


# ('{"float_num": 3.14}', {"float_num": 3.14}),


@pytest.mark.parametrize("json_str,expected_dict", BASIC_TEST_CASES)
def test_loads(json_str, expected_dict):
    """Проверка перевода json строки в python словарь"""
    result = cjson.loads(json_str)
    assert result == expected_dict


@pytest.mark.parametrize("expected_json,dict_data", BASIC_TEST_CASES)
def test_dumps(expected_json, dict_data):
    """Проверка сериализации словаря в строку"""
    result = cjson.dumps(dict_data)
    assert result == expected_json


@pytest.mark.parametrize("json_str,_", BASIC_TEST_CASES)
def test_roundtrip(json_str, _):
    """Проверка работы последовательности функций loads-dumps"""
    assert json_str == cjson.dumps(cjson.loads(json_str))


@pytest.mark.parametrize("_,json_dict", BASIC_TEST_CASES)
def test_roundtrip(_, json_dict):
    """Проверка работы последовательности функций dumps-loads"""
    assert json_dict == cjson.loads(cjson.dumps(json_dict))
