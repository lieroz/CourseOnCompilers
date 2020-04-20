#include <map>
#include <set>
#include <vector>
#include <iostream>

using Grammar = std::map<std::string, std::set<std::string>>;

static constexpr auto print = [](auto &&grammar) {
    for (auto &&[nonterm, rules]: grammar)
    {
        std::cout << "NON TERM: " << nonterm << " -> ";
        std::copy(std::begin(rules), std::end(rules),
            std::ostream_iterator<std::string>(std::cout, " | "));
        std::cout << '\n';
    }
};

static constexpr auto isRecursiveRule = [](auto &&rule, auto &&nonterm) {
    if (auto term = rule.substr(nonterm.size());
        rule.find_first_of(nonterm) == 0 && term.size() > 0)
    {
        return true;
    }
    return false;
};

Grammar eliminateLeftRecursion(const Grammar &grammar)
{
    auto newGrammar = grammar;
    std::vector<std::string> nonterms;

    std::transform(std::begin(newGrammar), std::end(newGrammar), std::back_inserter(nonterms),
        [](auto &&kv) { return kv.first; });

    for (size_t i = 0; i < nonterms.size(); ++i)
    {
        auto &&nontermI = nonterms[i];
        auto &&rulesI = newGrammar[nontermI];
        for (size_t j = 0; j < i; ++j)
        {
            auto &&nontermJ = nonterms[j];
            auto &&rulesJ = newGrammar[nontermJ];

            std::vector<std::string> filteredRules;
            std::copy_if(std::begin(rulesI), std::end(rulesI), std::back_inserter(filteredRules),
                [&](auto &&rule) { return isRecursiveRule(rule, nontermJ); });

            for (auto &&rule: filteredRules)
            {
                rulesI.erase(rule);
                auto term = rule.substr(nontermJ.size());

                std::for_each(std::begin(rulesJ), std::end(rulesJ),
                    [&](auto &&x) { rulesI.insert(x + term); });
            }
        }

        std::set<std::string> changedRules;
        std::set<std::string> newNontermRules;
        auto newNonterm = nontermI + "'";

        for (auto &&rule: rulesI)
        {
            if (isRecursiveRule(rule, nontermI))
            {
                auto term = rule.substr(nontermI.size());
                newNontermRules.insert({term + newNonterm, term});
            }
            else
            {
                changedRules.insert({rule + newNonterm, rule});
            }
        }

        if (!newNontermRules.empty())
        {
            newGrammar[nontermI] = std::move(changedRules);
            newGrammar[newNonterm] = std::move(newNontermRules);
        }
    }

    return newGrammar;
}

int main()
{
    Grammar grammar = {
        {"A", {"Sa"}}, {"S", {"Sb", "Ac", "b"}},
        /* {"E", {"E + T", "T"}}, */
        /* {"T", {"T * F", "F"}}, */
        /* {"F", {"a", "(E)"}}, */
    };

    auto newGrammar = eliminateLeftRecursion(grammar);
    print(newGrammar);

    return 0;
}
