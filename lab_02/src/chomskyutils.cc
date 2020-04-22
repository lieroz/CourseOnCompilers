#include "chomskyutils.h"

constexpr size_t longRuleSize = 2;

Grammar deleteLongRules(const Grammar &grammar)
{
    Grammar newGrammar;

    for (auto &&[nonterm, rules]: grammar)
    {
        for (auto &&rule: rules)
        {
            auto currentNonterm = nonterm;
            size_t i = 0;

            for (; i < rule.size() - 2; ++i)
            {
                auto nextNonterm = generateNewNonterm(grammar, currentNonterm);
                newGrammar[currentNonterm].insert(rule[i] + nextNonterm);
                currentNonterm = nextNonterm;
            }

            newGrammar[currentNonterm].insert(rule.substr(i));
        }
    }

    return newGrammar;
}
