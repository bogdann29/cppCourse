#include <gtest/gtest.h>
#include <iostream>
#include "format.h"

struct data {
   public:
    data(int a, std::string b) : a(a), b(b) {};
    friend std::ostream& operator<<(std::ostream& stream, const data& d) {
        stream << '[' << d.a << ',' << d.b << ']';
        return stream;
    }

   private:
    int a;
    std::string b;
};

TEST(Format, Basic) {
    EXPECT_EQ(format("{0}", 42), "42");
    EXPECT_EQ(format("{0}!", "Hello world"), "Hello world!");
    EXPECT_EQ(format("pi = {0}, e = {1}", 3.14, 2.71), "pi = 3.14, e = 2.71");

    EXPECT_EQ(format("{2}-{0}-{1}", 1, 2, 0), "0-1-2");
    EXPECT_EQ(format("{0}{0}{0}", "ha"), "hahaha");

    data d{7, "string"};
    EXPECT_EQ(format("user data object {0}", d), "user data object [7,string]");
}

TEST(Format, DifferentArgumentsNumber) {
    EXPECT_EQ(format("{0}", 42, "extra_argement"), "42");
    EXPECT_THROW(format("{0}-{1}-{2} third argument doesn't exist", 1, 2), myException);
    data d{0, "zero"};
    EXPECT_EQ(format("arguments 3, but we use only first = {0} and last = {2}", 42, "extra_argement", d),
              "arguments 3, but we use only first = 42 and last = [0,zero]");
    EXPECT_EQ(format("plain text"), "plain text");
}

TEST(Format, IncorrectUseBrackets) {
    EXPECT_THROW(format("in the end {0} closing bracket }", "extra"), myException);
    EXPECT_THROW(format("in the end {0} opening bracket {", "extra"), myException);
    EXPECT_THROW(format("good game { well {0}", "played"), myException);
    EXPECT_THROW(format("double breces {{0}}", true), myException);
}

TEST(Format, InvalidPlaceholderValue) {
    EXPECT_THROW(format("placeholder is not integer {0.5}", "wow"), myException);
    EXPECT_THROW(format("placeholder is string {abc}", "wow"), myException);
    EXPECT_THROW(format("placeholder is out of range {327891562783562781658721365821837956783216518956782}", "wow"), myException);
    EXPECT_THROW(format("placeholder is negative {-1}", "wow"), myException);
}