import json

import ujson

import cjson

import time
def main():
    with open("dataset/fake_data.json", 'r') as f:
        json_str = json.dumps(json.load(f))
        
        print(type(json_str))
    
        t = time.time()
        json_doc = json.loads(json_str)
        print(time.time() - t)
        
        t = time.time()
        ujson_doc = ujson.loads(json_str)
        print(time.time() - t)
        
        t = time.time()
        cjson_doc = cjson.loads(json_str)
        print(time.time() - t)
        
        assert json_doc == ujson_doc == cjson_doc
        
        t = time.time()
        assert json_str == cjson.dumps(cjson.loads(json_str))
        print(time.time() - t)
        
if __name__ == "__main__":
    main()

# import json
# from faker import Faker
# import random

# fake = Faker()

# def generate_fake_json(num_entries=1_000_000, filename="dataset/fake_data.json"):
#     data = {}
    
#     for i in range(num_entries):
#         key = f"{fake.word()}_{i}"  # Пример: "user_42", "product_123"
        
#         # Случайно выбираем, будет ли значение строкой или числом
#         if random.choice([True, False]):
#             value = fake.name()  # Или fake.city(), fake.email() и т. д.
#         else:
#             value = random.randint(1, 1000)  # Или random.uniform(1.0, 100.0)
        
#         data[key] = value
    
#     # Сохраняем в JSON-файл
#     with open(filename, "w") as f:
#         json.dump(data, f, indent=2 if num_entries < 10 else None)  # Красивое форматирование для маленьких файлов

#     print(f"Файл {filename} успешно создан! Записей: {num_entries}")

# # Пример использования
# if __name__ == "__main__":
#     generate_fake_json(num_entries=1_000_000)