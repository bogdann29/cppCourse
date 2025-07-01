#include "format.h"

void text_split(const std::string& str,
                std::vector<std::string>& text_parts,
                std::vector<uint32_t>& params_numbers) {
    int32_t counter = 0;
    auto part_begin = str.begin();
    auto number_begin = str.end();

    for (auto it = str.begin(); it != str.end(); ++it) {
        if (*it == '{') {
            if (++counter != 1)
                throw myException("Incorrect use bracket \'{\' or \'}\'");
            text_parts.emplace_back(part_begin, it);
            number_begin = next(it);
        } else if (*it == '}') {
            if (--counter != 0)
                throw myException("Incorrect use bracket \'{\' or \'}\'");
            auto s = std::string(number_begin, it);
            size_t pos = 0;
            try {
                // can throw std::invalid_argument or std::out_of_range
                auto tmp = static_cast<uint32_t>(std::stoull(s, &pos));
                if (pos != s.size())
                    throw myException("");
                params_numbers.push_back(tmp);
            } catch (const std::exception&) {
                throw myException("Placeholder must be an integer number");
            }
            part_begin = next(it);
        }
    }
    if (counter)
        throw myException("Incorrect use bracket \'{\' or \'}\'");

    text_parts.emplace_back(part_begin, str.end());
}