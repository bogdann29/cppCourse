#pragma once
#include <sstream>
#include <type_traits>

enum class Error {
    NoError,
    SerializeError,
    DeserializeError
};

template <typename T, typename Serializer>
struct has_serialize {
    template <typename U, typename S>
    static auto check(U* u) -> decltype(u->serialize(std::declval<S&>()));

    template <typename>
    static std::false_type check(...);
    static constexpr bool value = std::is_same<decltype(has_serialize::check<T, Serializer>((T*)nullptr)), Error>::value;
};

class Serializer {
   public:
    Serializer() = default;
    explicit Serializer(std::stringstream& stream) : data_(stream) {}

    Error operator()(bool arg);

    Error operator()(uint64_t arg);

    template <typename T, typename... Args>
    Error operator()(T&& arg, Args&&... args) {
        if (auto err = operator()(std::forward<T>(arg)); err != Error::NoError) {
            return err;
        }
        data_ << ' ';
        return operator()(std::forward<Args>(args)...);
    }

    std::string get_data() const {
        return data_.str();
    }

    template <typename T>
    Error save(T& data, typename std::enable_if<has_serialize<T, Serializer>::value>::type* = nullptr) {
        return data.serialize(*this);
    }

   private:
    std::stringstream& data_;
};

class Deserializer {
   public:
    Deserializer() = default;
    explicit Deserializer(std::stringstream& stream) : data_(stream) {}

    Error operator()(bool& arg);

    Error operator()(uint64_t& arg);

    template <typename T, typename... Args>
    Error operator()(T&& arg, Args&&... args) {
        if (auto err = operator()(std::forward<T>(arg)); err != Error::NoError)
            return err;
        return operator()(std::forward<Args>(args)...);
    }

    std::string get_data() const {
        return data_.str();
    }

    template <typename T>
    auto load(T& data, typename std::enable_if<has_serialize<T, Deserializer>::value>::type* = nullptr) {
        return data.serialize(*this);
    }

   private:
    std::stringstream& data_;
};

Error Serializer::operator()(bool arg) {
    data_ << std::boolalpha << arg;
    return data_.fail() ? Error::SerializeError : Error::NoError;
}

Error Serializer::operator()(uint64_t arg) {
    data_ << arg;
    return data_.fail() ? Error::SerializeError : Error::NoError;
}

Error Deserializer::operator()(bool& arg) {
    std::string s;
    if (!(data_ >> s))
        return Error::DeserializeError;

    if (s == "true") {
        arg = true;
    } else if (s == "false") {
        arg = false;
    } else
        return Error::DeserializeError;

    return Error::NoError;
}

Error Deserializer::operator()(uint64_t& arg) {
    std::string s;
    if (!(data_ >> s))
        return Error::DeserializeError;

    try {
        size_t pos = 0;
        auto tmp = static_cast<uint64_t>(std::stoull(s, &pos));
        if (pos != s.size())
            return Error::DeserializeError;
        arg = tmp;
    } catch (...) {
        return Error::DeserializeError;
    }

    return Error::NoError;
}