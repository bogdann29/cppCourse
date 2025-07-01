## Домашнее задание №6

Реализация функции для форматирования строки. Поддерживается любой тип, который может быть выведен в поток вывода. 
Формат строки форматирования:\

```
"{0} any text {1} {0}"
```

Номер в фигурных скобках - номер аргумента.

Пример:

```
auto text = format("{1}+{1} = {0}", 2, "one");
assert(text == "one+one = 2");
```

Для проверки:

- Клонирование:
```bash
    git clone https://github.com/bogdann29/cppCourse.git
```

- Сборка:
```bash
    cd cppCourse/task_6/
    cmake -S . -B build
    cd build
    make
```

- Запуск тестов
```bash
    ./tests/run_tests
```

- Запуск примера
```bash
    ./main
```