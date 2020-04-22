#include "leftutils.h"

int main()
{
    Grammar grammar = {
        {"A", {"Sa"}},
        {"S", {"Sb", "Ac", "b", epsilon}},
    };

    auto newGrammar1 = eliminateLeftRecursion(grammar);
    print(newGrammar1);

    auto newGrammar2 = leftFactoring(newGrammar1);
    print(newGrammar2);

    return 0;
}
