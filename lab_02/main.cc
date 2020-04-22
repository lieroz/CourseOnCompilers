#include "leftutils.h"
#include "chomskyutils.h"

int main()
{
    Grammar grammar = {
        {"S", {"aXbX", "aZ"}},
        {"X", {"aY", "bY", epsilon}},
        {"Y", {"X", "cc"}},
        {"Z", {"ZX"}},
    };

    auto newGrammar1 = deleteLongRules(grammar);
    print(newGrammar1);

    auto newGrammar2 = removeEpsilonNonterms(newGrammar1);
    print(newGrammar2);

    auto newGrammar3 = deleteChainRules(newGrammar2);
    print(newGrammar3);

    return 0;
}
