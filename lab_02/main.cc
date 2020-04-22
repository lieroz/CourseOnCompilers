#include "leftutils.h"

int main()
{
    Grammar grammar = {
        {"S", {"aS'", "aZ"}},
        {"X", {"aY", "bY", epsilon}},
        {"Y", {"X", "cc"}},
        {"Z", {"ZX"}},
        {"S'", {"XS''"}},
        {"S''", {"yX"}},
    };

    auto newGrammar = removeEpsilonNonterms(grammar);
    print(newGrammar);

    /* auto newGrammar1 = eliminateLeftRecursion(grammar); */
    /* print(newGrammar1); */

    /* auto newGrammar2 = leftFactoring(newGrammar1); */
    /* print(newGrammar2); */

    return 0;
}
