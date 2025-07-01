#pragma once
#include <exception>
#include <string>

class myException : public std::exception {
   public:
    explicit myException(const std::string& message) : std::exception(), message_(message) {};
    const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override { return message_.c_str(); }

   private:
    std::string message_;
};