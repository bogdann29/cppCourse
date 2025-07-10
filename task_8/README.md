# Домашнее задание #08 (Расширения на C)

Реализация библиотеки для парсинга и сериализации json (на CAPI)

Библиотека поддерживает 2 функции: loads и dumps

- loads принимает на вход строку в формате JSON. Возвращает объект типа dict.
- dumps принимает на вход объект типа dict и возвращает строку.

Пример:

```py
import cjson 

json_str = '{"hello": 10, "world": "value"}'
cjson_doc = cjson.loads(json_str)
assert cjson_doc == {"hello": 10, "world": "value"}
assert json_str == cjson.dumps(cjson.loads(json_str))
```

Для проверки:

- Клонирование:
```bash
    git clone https://github.com/bogdann29/cppCourse.git
```

- Сборка:
```bash
    cd cppCourse/task_8/
    mkdir build
    cd build
    cmake ..
```

- Запуск тестов
```bash
    make test
```

- Запуск бенчмарка
```bash
    make benchmark
```

### Примечание
* Генератор для данных - tests/generate_data.py
* Данные уже сгенерированны и находятся в директории dataset
* Таргет в cmake для автомотической генерации не заработал, данные долго генерируются, поэтому они оставлены в репозитории

- Для ручной генерации данных (из основной директории)
```bash
    python3 tests/generate_data.py
```
