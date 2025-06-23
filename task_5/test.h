#pragma once
#include <cassert>
#include <iostream>
#include <limits>
#include <string>

#include "serializer.h"

struct Computer {
    uint64_t cores;
    uint64_t ram_gb;
    bool is_gaming;
    uint64_t storage_gb;
    bool has_wifi;

    template <class Serializer>
    Error serialize(Serializer& serializer) {
        return serializer(cores, ram_gb, is_gaming, storage_gb, has_wifi);
    }
};

void test_serialization() {
    {
        Computer pc1{8, 16, true, 1000, false};
        std::stringstream stream;

        Serializer ser(stream);
        assert(ser.save(pc1) == Error::NoError);

        Computer pc2{};
        Deserializer des(stream);
        assert(des.load(pc2) == Error::NoError);

        assert(pc1.cores == pc2.cores);
        assert(pc1.ram_gb == pc2.ram_gb);
        assert(pc1.is_gaming == pc2.is_gaming);
        assert(pc1.storage_gb == pc2.storage_gb);
        assert(pc1.has_wifi == pc2.has_wifi);
    }

    // Граничные значения чисел
    {
        Computer pc1{
            std::numeric_limits<uint64_t>::max(),
            std::numeric_limits<uint64_t>::max(),
            false,
            0,
            true};
        std::stringstream stream;

        Serializer ser(stream);
        assert(ser.save(pc1) == Error::NoError);

        Computer pc2{};
        Deserializer des(stream);
        assert(des.load(pc2) == Error::NoError);
        assert(pc2.cores == UINT64_MAX);
        assert(pc2.storage_gb == 0);
    }

    // Все возможные комбинации bool
    {
        const bool test_flags[2] = {false, true};
        for (bool flag1 : test_flags) {
            for (bool flag2 : test_flags) {
                Computer pc1{4, 8, flag1, 500, flag2};
                std::stringstream stream;

                Serializer ser(stream);
                assert(ser.save(pc1) == Error::NoError);

                Computer pc2{};
                Deserializer des(stream);
                assert(des.load(pc2) == Error::NoError);
                assert(pc1.is_gaming == pc2.is_gaming);
                assert(pc1.has_wifi == pc2.has_wifi);
            }
        }
    }

    // Неверный формат
    {
        // Булево значение вместо числа
        std::stringstream bad_bool_stream;
        bad_bool_stream << "4 16 2 500 1";  // is_gaming = 2

        Computer pc{};
        Deserializer des1(bad_bool_stream);
        assert(des1.load(pc) == Error::DeserializeError);

        // Строка вместо числа
        std::stringstream bad_num_stream;
        bad_num_stream << "four 16 true 500 false";  // cores = "four"

        Deserializer des2(bad_num_stream);
        assert(des2.load(pc) == Error::DeserializeError);
    }

    // Недостаток/избыток данных
    {
        // Не хватает полей
        std::stringstream short_stream;
        short_stream << "4 16 true";

        Computer pc1{};
        Deserializer des1(short_stream);
        assert(des1.load(pc1) == Error::DeserializeError);

        // Лишние данные
        std::stringstream long_stream;
        long_stream << "4 16 true 500 false extra_data";

        Computer pc2{};
        Deserializer des2(long_stream);
        assert(des2.load(pc2) == Error::NoError);
    }

    // Ошибки сериализации
    {
        Computer pc1{8, 16, true, 1000, false};
        std::stringstream bad_stream;
        bad_stream.setstate(std::ios::failbit);  // Ошибка потока

        Serializer ser(bad_stream);
        assert(ser.save(pc1) == Error::SerializeError);
        assert(ser(static_cast<uint64_t>(100)) == Error::SerializeError);

        Computer pc2{};
        Deserializer des(bad_stream);
        assert(des.load(pc2) == Error::DeserializeError);
    }
}

inline void run_all_tests() {
    test_serialization();
    std::cout << "All serialization tests passed!" << std::endl;
}