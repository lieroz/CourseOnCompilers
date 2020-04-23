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

    print(grammar);

    std::cout << "CHOMSKY\n";
    std::cout << "===\n";

    auto grammarWithoutLongRules = deleteLongRules(grammar);
    print(grammarWithoutLongRules);

    auto grammarWithoutEpsilonNonterminals = removeEpsilonNonterms(grammarWithoutLongRules);
    print(grammarWithoutEpsilonNonterminals);

    auto grammarWithoutChainRules = deleteChainRules(grammarWithoutEpsilonNonterminals);
    print(grammarWithoutChainRules);

    std::cout << "LEFT RECURSION ELIMINATION\n";
    std::cout << "===\n";

    auto grammarWithoutLeftRecursion = eliminateLeftRecursion(grammar);
    print(grammarWithoutLeftRecursion);

    std::cout << "LEFT FACTORING\n";
    std::cout << "===\n";

    auto leftFactoredGrammar = leftFactoring(grammarWithoutLeftRecursion);
    print(leftFactoredGrammar);

    return 0;
}
