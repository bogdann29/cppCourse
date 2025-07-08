import pytest
import cjson
from pytest import raises


def test_empty_input():
    """Проверка пустого ввода"""
    with raises(ValueError):
        cjson.loads("")


def test_invalid_json_format():
    """Проверка некорректного формата JSON"""
    test_cases = [
        '{"key": "value"',      # Не хватает "}"
        '"key": "value"}',      # Не хватает "{"
        '{key: "value"}',       # Ключ без кавычек
        "{'key': 'value'}",     # Неверные кавычки
        '{"key": value}',       # Значение без кавычек и не число
        '{"key": "value",}',    # Висячая запятая
        "{invalid}",            # Полностью невалидный JSON
    ]

    for invalid_json in test_cases:
        with raises(ValueError):
            cjson.loads(invalid_json)


def test_invalid_key_value_pairs():
    """Проверка некорректных пар ключ-значение"""
    test_cases = [
        '{"key":}',             # Пропущено значение
        '{:"value"}',           # Пропущен ключ
        '{"key" "value"}',      # Пропущено двоеточие
        '{"key": : "value"}',   # Некорректное двоеточие
    ]

    for invalid_pair in test_cases:
        with raises(ValueError):
            cjson.loads(invalid_pair)


def test_invalid_numbers():
    """Проверка некорректных числовых значений"""
    test_cases = [
        '{"num": 123abc}',      # Не число
        '{"num": 12.34.56}',    # Неправильный формат числа
        '{"num": 1e1000}',      # Неправильный формат числа
        '{"num": -}',           # Неполное отрицательное число
    ]

    for invalid_num in test_cases:
        with raises(ValueError):
            cjson.loads(invalid_num)


def test_invalid_string_escaping():
    """Проверка некорректного экранирования в строках"""
    with raises(ValueError):
        cjson.loads('{"str": "invalid\\escape"}')


def test_empty_key_or_value():
    """Проверка пустых ключей или значений"""
    with raises(ValueError):
        cjson.loads('{"": "value"}')        # Пустой ключ
    with raises(ValueError):
        cjson.loads('{"key": ""}')          # Пустое значение


def test_type_mismatch():
    """Проверка несоответствия типов"""
    with raises(ValueError):
        cjson.loads('{"key": [1,2,3]}')     # Массивы
    with raises(ValueError):
        cjson.loads(
            '{"key": {"nested": "value"}}'
        )                                   # Вложенные объекты
