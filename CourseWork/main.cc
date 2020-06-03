#include <iostream>

#include "lisp.h"

int main()
{
    lisp::repl(std::cin, std::cout, "lispy> ", lisp::init());
    return 0;
}
