#include <iostream>

#include "parser.h"

int main()
{
    auto &&[f, r] = parse({"a", "and", "c", "xor", "(", "b", "or", "c", ")", "and", "1"});
    std::cout << std::boolalpha << f << ": " << r << std::endl;

    return 0;
}
