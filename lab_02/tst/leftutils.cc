#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "leftutils.h"

TEST(eliminateLeftRecursion, TestsThatLeftRecursionIsEliminated)
{
    Grammar grammar = {
        {"A", {"Sa"}},
        {"S", {"Sb", "Ac", "b", epsilon}},
    };

    auto newGrammar = eliminateLeftRecursion(grammar);
    ASSERT_THAT(newGrammar["A"], testing::ElementsAre("Sa", "a"));
    ASSERT_THAT(newGrammar["S"], testing::ElementsAre("ac", "acS'", "b", "bS'"));
    ASSERT_THAT(newGrammar["S'"], testing::ElementsAre("ac", "acS'", "b", "bS'"));
}

TEST(leftFactoring, TestsThatLeftFactoringIsCorrect)
{
    Grammar grammar = {
        {"A", {"Sa"}},
        {"S", {"Sb", "Ac", "b", epsilon}},
    };

    auto newGrammar = leftFactoring(eliminateLeftRecursion(grammar));
    ASSERT_THAT(newGrammar["A"], testing::ElementsAre("SaA'", "aA''"));
    ASSERT_THAT(newGrammar["A'"], testing::ElementsAre("?"));
    ASSERT_THAT(newGrammar["A''"], testing::ElementsAre("?"));
    ASSERT_THAT(newGrammar["S"], testing::ElementsAre("acS''", "bS'''"));
    ASSERT_THAT(newGrammar["S'"], testing::ElementsAre("acS''''", "bS'''''"));
    ASSERT_THAT(newGrammar["S''"], testing::ElementsAre("?", "S'"));
    ASSERT_THAT(newGrammar["S'''"], testing::ElementsAre("?", "S'"));
    ASSERT_THAT(newGrammar["S''''"], testing::ElementsAre("?", "S'"));
    ASSERT_THAT(newGrammar["S'''''"], testing::ElementsAre("?", "S'"));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
