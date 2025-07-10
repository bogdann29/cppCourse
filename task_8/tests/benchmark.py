import json
import ujson
import cjson
import timeit
import pytest
from pathlib import Path

# Количество запусков для усреднения
NUM_RUNS = 10


class TestPerformance:
    @classmethod
    def setup_class(cls):
        cls.test_files = sorted(Path("dataset").glob("fake_data_*.json"))
        if not cls.test_files:
            pytest.skip("No test data files found in dataset directory")

    def get_avg_time(self, func, *args):
        """Возвращает среднее время выполнения функции"""
        timer = timeit.Timer(lambda: func(*args))
        return timer.timeit(number=NUM_RUNS) / NUM_RUNS

    @pytest.mark.parametrize(
        "test_file", [f.name for f in Path("dataset").glob("fake_data_*.json")]
    )
    def test_file_performance(self, test_file):
        with open(f"dataset/{test_file}", "r") as f:
            data = f.read()

        print(f"\n=== Benchmarking {test_file} ===")

        json_load = self.get_avg_time(json.loads, data)
        ujson_load = self.get_avg_time(ujson.loads, data)
        cjson_load = self.get_avg_time(cjson.loads, data)

        json_obj = json.loads(data)

        json_dump = self.get_avg_time(json.dumps, json_obj)
        ujson_dump = self.get_avg_time(ujson.dumps, json_obj)
        cjson_dump = self.get_avg_time(cjson.dumps, json_obj)

        print("\nLoad performance (avg seconds):")
        print(f"json:  {json_load:.6f}")
        print(f"ujson: {ujson_load:.6f}")
        print(f"cjson: {cjson_load:.6f}")

        print("\nDump performance (avg seconds):")
        print(f"json:  {json_dump:.6f}")
        print(f"ujson: {ujson_dump:.6f}")
        print(f"cjson: {cjson_dump:.6f}")
