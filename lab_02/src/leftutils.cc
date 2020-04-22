#include "leftutils.h"

Grammar eliminateLeftRecursion(const Grammar &grammar)
{
    auto newGrammar = removeEpsilonNonterms(grammar);
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
        auto newNonterm = generateNewNonterm(newGrammar, nontermI);

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

Grammar leftFactoring(const Grammar &grammar)
{
    auto newGrammar = grammar;

    for (auto &&[nonterm, rules]: grammar)
    {
        if (rules.size() > 1)
        {
            for (auto &&prefix: findAllCommonPrefixes(rules))
            {
                auto newNonterm = generateNewNonterm(newGrammar, nonterm);
                newGrammar[nonterm].insert(prefix + newNonterm);

                for (auto &&rule: rules)
                {
                    if (auto pos = rule.find(prefix); pos == 0)
                    {
                        auto expr = rule.substr(prefix.size());
                        newGrammar[newNonterm].insert(expr.empty() ? epsilon : expr);
                        newGrammar[nonterm].erase(rule);
                    }
                }
            }
        }
    }

    return newGrammar;
}
