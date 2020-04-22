#include "leftutils.h"

int main()
{
    Grammar grammar = {
        {"A", {"Sa"}}, {"S", {"Sb", "Ac", "b", epsilon}},
        /* {"E", {"E + T", "T"}}, */
        /* {"T", {"T * F", "F"}}, */
        /* {"F", {"a", "(E)"}}, */
    };

    auto newGrammar1 = eliminateLeftRecursion(grammar);
    print(newGrammar1);

    auto newGrammar2 = leftFactoring(newGrammar1);
    print(newGrammar2);

    /* std::set<std::string> arr = {"Sa", "Sb", "Aa", "Ab"}; */
    /* findAllCommonPrefixes(arr); */

    /* Grammar epsGrammar = { */
    /*     {"S", {"ABCd"}}, */
    /*     {"A", {"a", epsilon}}, */
    /*     {"B", {"AC"}}, */
    /*     {"C", {"c", epsilon}}, */
    /* }; */

    /* auto epsilonNonterms = findEpsilonNonterms(epsGrammar); */
    /* std::copy(std::begin(epsilonNonterms), std::end(epsilonNonterms), */
    /*     std::ostream_iterator<std::string>(std::cout, " ")); */
    /* std::cout << std::endl; */

    /* auto permutations = findAllPermutations(epsilonNonterms, "eABCdGe"); */
    /* auto permutations = findAllPermutations({}, "eABCdGe"); */
    /* std::copy(std::begin(permutations), std::end(permutations), */
    /*     std::ostream_iterator<std::string>(std::cout, " ")); */
    /* std::cout << std::endl; */

    return 0;
}
