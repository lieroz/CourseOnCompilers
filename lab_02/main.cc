#include <map>
#include <set>
#include <vector>
#include <queue>
#include <iostream>

static inline const char *epsilon = "?";

using Grammar = std::map<std::string, std::set<std::string>>;

static constexpr auto print = [](auto &&grammar) {
    std::cout << "GRAMMAR:\n";
    std::cout << "===\n";
    for (auto &&[nonterm, rules]: grammar)
    {
        std::cout << "nonterm: " << nonterm << " -> ";
        std::copy(std::begin(rules), --std::end(rules),
            std::ostream_iterator<std::string>(std::cout, " | "));
        std::cout << *(--std::end(rules)) << '\n';
    }
    std::cout << "===\n\n";
};

static constexpr auto isRecursiveRule = [](auto &&rule, auto &&nonterm) {
    if (auto term = rule.substr(nonterm.size());
        rule.find_first_of(nonterm) == 0 && term.size() > 0)
    {
        return true;
    }
    return false;
};

static constexpr auto generateNewNonterm = [](auto &&grammar, auto &&nonterm) {
    auto newNonterm = nonterm;
    do
    {
        newNonterm += "'";
    } while (grammar.find(newNonterm) != std::end(grammar));
    return newNonterm;
};

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

static constexpr auto commonPrefixUtil = [](auto &&a, auto &&b) {
    std::string result;
    auto itA = std::begin(a);
    auto itB = std::begin(b);
    for (; (itA != std::end(a) && itB != std::end(b)) && (*itA == *itB); ++itA, ++itB)
    {
        result.push_back(*itA);
    }
    return result;
};

static constexpr auto commonPrefix = [](auto &&begin, auto &&end) {
    auto prefix = *begin;
    std::for_each(begin, end, [&](auto &&str) { prefix = commonPrefixUtil(prefix, str); });
    return prefix;
};

static constexpr auto findAllCommonPrefixes = [](auto &&container) {
    std::vector<std::string> prefixes;

    auto begin = std::begin(container);
    auto end = std::end(container);

    for (; begin != std::end(container);)
    {
        if (auto prefix = commonPrefix(begin, end); prefix.empty())
        {
            --end;
        }
        else
        {
            prefixes.push_back(prefix);
            begin = end;
            end = std::end(container);
        }
    }

    return prefixes;
};

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
