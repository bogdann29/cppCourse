import json
from faker import Faker
import random
from concurrent.futures import ThreadPoolExecutor
from threading import Lock
import time
from pathlib import Path


def generate_test_files(output_dir, num_entries=1_000_000, num_files=5):
    output_path = Path(output_dir)
    output_path.mkdir(exist_ok=True)
    
    for i in range(1, num_files + 1):
        generate_fake_json(num_entries, filename=f"{output_path}/fake_data_{i}.json")


def generate_fake_json(
    num_entries=1_000_000, filename="dataset/fake_data.json", num_workers=8
):
    fake = Faker()
    data = {}
    lock = Lock()

    def generate_chunk(start, end):
        chunk = {}
        for i in range(start, end):
            key = f"{fake.word()}_{i}"
            value = (
                fake.name() if random.choice([True, False]) else random.randint(1, 1000)
            )
            chunk[key] = value
        return chunk

    chunk_size = num_entries // num_workers
    chunks = [(i * chunk_size, (i + 1) * chunk_size) for i in range(num_workers)]
    chunks[-1] = (chunks[-1][0], num_entries)  # Корректируем последний чанк

    start_time = time.time()

    with ThreadPoolExecutor(max_workers=num_workers) as executor:
        futures = [executor.submit(generate_chunk, start, end) for start, end in chunks]
        for future in futures:
            chunk = future.result()
            with lock:
                data.update(chunk)

    with open(filename, "w") as f:
        json.dump(data, f)

    elapsed = time.time() - start_time
    print(
        f"Файл создан: {filename}\nЗаписей: {num_entries:,}\nВремя: {elapsed:.2f} сек"
    )


if __name__ == "__main__":
    generate_test_files(output_dir="dataset/")
