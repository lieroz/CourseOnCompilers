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

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
