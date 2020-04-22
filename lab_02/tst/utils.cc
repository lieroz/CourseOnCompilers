#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "utils.h"

TEST(isRecursiveRule, TestsThatRuleIsRecursive)
{
    std::string rule = "Aa";
    std::string nonterm = "A";

    EXPECT_TRUE(isRecursiveRule(rule, nonterm));
}

TEST(isRecursiveRule, TestsThatRuleIsNotRecursive)
{
    std::string rule = "aAa";
    std::string nonterm = "A";

    EXPECT_FALSE(isRecursiveRule(rule, nonterm));
}

TEST(isRecursiveRule, TestsThatMultiRuleIsRecursive)
{
    std::string rule = "aAa";
    std::string nonterm = "aA";

    EXPECT_TRUE(isRecursiveRule(rule, nonterm));
}

TEST(generateNewNonterm, TestsThatNewNontermIsGenerated)
{
    std::string nonterm = "S";
    Grammar grammar = {{nonterm, {}}};
    EXPECT_EQ("S'", generateNewNonterm(grammar, nonterm));
}

TEST(generateNewNonterm, TestsThatNewNestedNontermIsGenerated)
{
    std::string nonterm = "S'''";
    Grammar grammar = {{nonterm, {}}};
    EXPECT_EQ("S''''", generateNewNonterm(grammar, nonterm));
}

TEST(findAllCommonPrefixes, TestsThatAllCommonPrefixesAreFound)
{
    std::vector<std::string> container = {"Sa", "Sac", "Aa", "a"};
    ASSERT_THAT(findAllCommonPrefixes(container), testing::ElementsAre("Sa", "Aa", "a"));
}

TEST(ruleHasTerms, TestsThatRuleHasTerms)
{
    Grammar grammar = {
        {"A", {}},
        {"B", {}},
        {"C", {}},
        {"D", {}},
    };
    std::string rule = "eABCdGe";

    EXPECT_TRUE(ruleHasTerms(grammar, rule));
}

TEST(ruleHasTerms, TestsThatRuleHasNoTerms)
{
    Grammar grammar = {
        {"A", {}},
        {"B", {}},
        {"C", {}},
        {"G", {}},
    };
    std::string rule = "ABCG";

    EXPECT_FALSE(ruleHasTerms(grammar, rule));
}

TEST(findEpsilonNonterms, TestsThatAllEpsilonNonTermsAreFound)
{
    Grammar grammar = {
        {"S", {"ABCd"}},
        {"A", {"a", epsilon}},
        {"B", {"AC"}},
        {"C", {"c", epsilon}},
    };

    ASSERT_THAT(findEpsilonNonterms(grammar), testing::ElementsAre("A", "B", "C"));
}

TEST(findAllPermutations, TestsThatAllPermutationsAreFound)
{
    Grammar grammar = {
        {"S", {"ABCd"}},
        {"A", {"a", epsilon}},
        {"B", {"AC"}},
        {"C", {"c", epsilon}},
        {"G", {epsilon}},
    };

    auto epsilonNonterms = findEpsilonNonterms(grammar);

    ASSERT_THAT(findAllPermutations(epsilonNonterms, "eABCdGe"),
        testing::ElementsAre("eABCdGe", "eABCde", "eABdGe", "eABde", "eACdGe", "eACde", "eAdGe",
            "eAde", "eBCdGe", "eBCde", "eBdGe", "eBde", "eCdGe", "eCde", "edGe", "ede"));
    ASSERT_THAT(findAllPermutations({}, "eABCdGe"), testing::ElementsAre("eABCdGe"));
}

TEST(removeEpsilonNonterms, TestsThatEpsilonNonTermsAreRemoved)
{
    Grammar grammar = {
        {"A", {"Sa"}}, {"S", {"Sb", "Ac", "b", epsilon}},
    };

    auto newGrammar = removeEpsilonNonterms(grammar);
    ASSERT_THAT(newGrammar["A"], testing::ElementsAre("Sa", "a"));
    ASSERT_THAT(newGrammar["S"], testing::ElementsAre("Ac", "Sb", "b"));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
