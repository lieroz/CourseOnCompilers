#include "utils.h"

#include <queue>

bool ruleHasTerms(const Grammar &grammar, std::string rule)
{
    for (auto &&[nonterm, _]: grammar)
    {
        auto pos = rule.find(nonterm);
        if (pos != std::string::npos)
        {
            rule.erase(pos, nonterm.size());
        }
    }
    return !rule.empty();
}

std::set<std::string> findEpsilonNonterms(const Grammar &grammar)
{
    std::queue<std::string> epsilonNonterms;
    std::map<std::string, std::string> reversedGrammar;
    std::map<std::string, size_t> rulesCounters;
    std::map<std::string, std::set<std::string>> concernedRules;

    for (auto &&[nontermI, rulesI]: grammar)
    {
        for (auto &&[nontermJ, rulesJ]: grammar)
        {
            for (auto &&rule: rulesJ)
            {
                if (!ruleHasTerms(grammar, rule) && rule.find(nontermI) != std::string::npos)
                {
                    concernedRules[nontermI].insert(rule);
                    ++rulesCounters[rule];
                }
            }
        }

        for (auto &&rule: rulesI)
        {
            if (rule == epsilon)
            {
                epsilonNonterms.push(nontermI);
            }
            else
            {
                reversedGrammar.emplace(rule, nontermI);
            }
        }
    }

    std::set<std::string> result;
    for (; !epsilonNonterms.empty();)
    {
        auto nonterm = epsilonNonterms.front();
        epsilonNonterms.pop();

        for (auto &&rule: concernedRules[nonterm])
        {
            auto &&counter = --rulesCounters[rule];
            if (counter == 0)
            {
                epsilonNonterms.push(reversedGrammar[rule]);
            }
        }

        result.insert(nonterm);
    }

    return result;
}

std::set<std::string> findAllPermutations(
    const std::set<std::string> &epsilonNonterms, std::string rule)
{
    std::vector<std::string> decomposed;

    for (auto itI = std::begin(rule); itI != std::end(rule); ++itI)
    {
        std::string str;
        str = *itI;

        for (auto itJ = itI + 1; itJ != std::end(rule); ++itJ)
        {
            if (epsilonNonterms.find(str) == std::end(epsilonNonterms))
            {
                str += *itJ;
            }
        }

        if (epsilonNonterms.find(str) == std::end(epsilonNonterms))
        {
            str = *itI;
        }
        else
        {
            itI += str.size() - 1;
        }

        decomposed.push_back(str);
    }

    std::set<std::string> result;
    result.insert("");

    for (auto &&i: decomposed)
    {
        std::set<std::string> newResult;
        if (epsilonNonterms.find(i) != std::end(epsilonNonterms))
        {
            newResult = result;
        }

        for (auto &&j: result)
        {
            newResult.insert(j + i);
        }
        std::swap(result, newResult);
    }

    return result;
}

Grammar removeEpsilonNonterms(const Grammar &grammar)
{
    Grammar newGrammar;
    auto epsilonNonterms = findEpsilonNonterms(grammar);

    for (auto &&[nonterm, rules]: grammar)
    {
        for (auto &&rule: rules)
        {
            if (rule != epsilon)
            {
                auto permutations = findAllPermutations(epsilonNonterms, rule);
                newGrammar[nonterm].insert(std::begin(permutations), std::end(permutations));
            }
        }
    }

    return newGrammar;
}
