#include <iostream>
#include "format.h"

int main() {
    auto example = format("{1}+{1} = {0}", 2, "one");
    std::cout << "example format(\"{1}+{1} = {0}\", 2, \"one\") returns :\n" << example << std::endl;
    return 0;
}