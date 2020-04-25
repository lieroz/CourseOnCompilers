#pragma once

#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>

using Grammar = std::map<std::string, std::set<std::string>>;

inline const char *epsilon = "?";

inline auto print = [](auto &&grammar) {
    std::cout << "GRAMMAR:\n";
    std::cout << "===\n";
    for (auto &&[nonterm, rules]: grammar)
    {
        std::cout << "nonterm: " << nonterm << " ->  rules: ";
        std::copy(std::begin(rules), --std::end(rules),
            std::ostream_iterator<std::string>(std::cout, " | "));
        std::cout << *(--std::end(rules)) << '\n';
    }
    std::cout << "===\n\n";
};

constexpr auto isRecursiveRule = [](auto &&rule, auto &&nonterm) {
    if (auto term = rule.substr(nonterm.size());
        rule.find_first_of(nonterm) == 0 && term.size() > 0)
    {
        return true;
    }
    return false;
};

constexpr auto generateNewNonterm = [](auto &&grammar, auto &&nonterm) {
    auto newNonterm = nonterm;
    do
    {
        newNonterm += "'";
    } while (grammar.find(newNonterm) != std::end(grammar));
    return newNonterm;
};

constexpr auto commonPrefixUtil = [](auto &&a, auto &&b) {
    std::string result;
    auto itA = std::begin(a);
    auto itB = std::begin(b);
    for (; (itA != std::end(a) && itB != std::end(b)) && (*itA == *itB); ++itA, ++itB)
    {
        result.push_back(*itA);
    }
    return result;
};

constexpr auto commonPrefix = [](auto &&begin, auto &&end) {
    auto prefix = *begin;
    std::for_each(begin, end, [&](auto &&str) { prefix = commonPrefixUtil(prefix, str); });
    return prefix;
};

constexpr auto findAllCommonPrefixes = [](auto &&container) {
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

bool ruleHasTerms(const Grammar &grammar, std::string rule);

std::set<std::string> findEpsilonNonterms(const Grammar &grammar);
std::set<std::string> findAllPermutations(
    const std::set<std::string> &epsilonNonterms, std::string rule);
Grammar removeEpsilonNonterms(const Grammar &grammar);
