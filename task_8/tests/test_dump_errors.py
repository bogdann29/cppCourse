import pytest
import cjson
from pytest import raises


def test_non_dict_input():
    """Проверка, что функция принимает только словари"""
    test_cases = [
        None,  # None
        "some string",  # строка
        123,  # число
        [1, 2, 3],  # список
        (1, 2, 3),  # кортеж
        True,  # булево значение
    ]

    for invalid_input in test_cases:
        with raises(TypeError):
            cjson.dumps(invalid_input)


def test_non_string_convertible_keys():
    """Проверка ключей, которые нельзя преобразовать в строку"""

    class NonStringKey:
        def __str__(self):
            raise TypeError("Cannot convert to string")

    invalid_dict = {NonStringKey(): "value"}

    with raises(TypeError):
        cjson.dumps(invalid_dict)


def test_non_string_convertible_values():
    """Проверка значений, которые нельзя преобразовать в строку"""

    class NonStringValue:
        def __str__(self):
            raise TypeError("Cannot convert to string")

    invalid_dict = {"key": NonStringValue()}

    with raises(TypeError):
        cjson.dumps(invalid_dict)
