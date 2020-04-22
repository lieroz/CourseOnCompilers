#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "chomskyutils.h"

TEST(deleteLongRules, TestsEliminationOfLongRules)
{
    Grammar grammar = {
        {"S", {"AB"}},
        {"A", {"aBcB"}},
        {"B", {"def"}},
    };

    auto newGrammar = deleteLongRules(grammar);
    ASSERT_THAT(newGrammar["A"], testing::ElementsAre("aA'"));
    ASSERT_THAT(newGrammar["A'"], testing::ElementsAre("BA''"));
    ASSERT_THAT(newGrammar["A''"], testing::ElementsAre("cB"));
    ASSERT_THAT(newGrammar["B"], testing::ElementsAre("dB'"));
    ASSERT_THAT(newGrammar["B'"], testing::ElementsAre("ef"));
    ASSERT_THAT(newGrammar["S"], testing::ElementsAre("AB"));
}

TEST(deleteLongRules, TestsEliminationOfLongRules2)
{
    Grammar grammar = {
        {"S", {"aXbX", "aZ"}},
        {"X", {"aY", "bY", epsilon}},
        {"Y", {"X", "cc"}},
        {"Z", {"ZX"}},
    };

    auto newGrammar = deleteLongRules(grammar);
    ASSERT_THAT(newGrammar["S"], testing::ElementsAre("aS'", "aZ"));
    ASSERT_THAT(newGrammar["S'"], testing::ElementsAre("XS''"));
    ASSERT_THAT(newGrammar["S''"], testing::ElementsAre("bX"));
    ASSERT_THAT(newGrammar["X"], testing::ElementsAre(epsilon, "aY", "bY"));
    ASSERT_THAT(newGrammar["Y"], testing::ElementsAre("X", "cc"));
    ASSERT_THAT(newGrammar["Z"], testing::ElementsAre("ZX"));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
