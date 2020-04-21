#include <map>
#include <set>
#include <vector>
#include <iostream>

using Grammar = std::map<std::string, std::set<std::string>>;

static constexpr auto print = [](auto &&grammar) {
    for (auto &&[nonterm, rules]: grammar)
    {
        std::cout << "nonterm: " << nonterm << " -> ";
        std::copy(std::begin(rules), --std::end(rules),
            std::ostream_iterator<std::string>(std::cout, " | "));
        std::cout << *(--std::end(rules)) << '\n';
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

static constexpr auto generateNewNonterm = [](auto &&grammar, auto &&nonterm) {
    auto newNonterm = nonterm;
    do
    {
        newNonterm += "'";
    } while (grammar.find(newNonterm) != std::end(grammar));
    return newNonterm;
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
                std::cout << prefix << std::endl;
                auto newNonterm = generateNewNonterm(newGrammar, nonterm);
                newGrammar[nonterm].insert(prefix + newNonterm);

                for (auto &&rule: rules)
                {
                    if (rule.find_first_of(prefix) == 0)
                    {
                        auto expr = rule.substr(prefix.size());
                        newGrammar[newNonterm].insert(expr.empty() ? "?" : expr);
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
        {"A", {"Sa"}}, {"S", {"Sb", "Ac", "b"}},
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

    return 0;
}
