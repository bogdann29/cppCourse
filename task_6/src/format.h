#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include "myException.h"

template <typename T>
concept CanBeOut =
    requires(std::ostream& stream, T&& t) {
        { stream << std::forward<T>(t) } -> std::same_as<std::ostream&>;
    };

void text_split(const std::string& str,
                std::vector<std::string>& text_parts,
                std::vector<uint32_t>& params_numbers);

template <typename T>
std::string make_string(T&& t) {
    std::ostringstream stream;
    stream << std::forward<T>(t);
    return stream.str();
}

template <CanBeOut... Args>
std::string format(const std::string& str, Args&&... args) {
    constexpr size_t args_count = sizeof...(args);

    std::vector<std::string> text_parts;
    std::vector<uint32_t> params_numbers;

    try {
        text_split(str, text_parts, params_numbers);
    } catch (const myException& ex) {
        throw ex;
    } catch (const std::exception& ex) {
        throw myException("Uncknown exception : " + std::string(ex.what()));
    }

    if (!params_numbers.empty() && *std::max_element(params_numbers.begin(), params_numbers.end()) >= args_count)
        throw myException("The number of parametrs doesn't match the number of arguments");

    std::array<std::string, args_count> arg_strings = {make_string(std::forward<Args>(args))...};

    std::string result = "";
    for (size_t i = 0; i < params_numbers.size(); ++i) {
        result += text_parts[i] + arg_strings[params_numbers[i]];
    }
    result += text_parts.back();
    return result;
}