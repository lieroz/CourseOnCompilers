#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "parser.h"

TEST(ParserTest, Test1)
{
    auto &&[f, r] = parse({"a", "<", "b"});

    EXPECT_TRUE(f);
    EXPECT_EQ(r, "a b <");
}

TEST(ParserTest, Test2)
{
    auto &&[f, r] = parse({"a", "b", "c"});

    EXPECT_FALSE(f);
    EXPECT_TRUE(r.empty());
}

TEST(ParserTest, Test3)
{
    auto &&[f, r] = parse({"(", "(", "(", "a", "/", "b", ")", ")", ")"});

    EXPECT_TRUE(f);
    EXPECT_EQ(r, "a b /");
}

TEST(ParserTest, Test4)
{
    auto &&[f, r] = parse({"(", "a", "mod", "b", ")", "&", "c"});

    EXPECT_TRUE(f);
    EXPECT_EQ(r, "a b mod c &");
}

TEST(ParserTest, Test5)
{
    auto &&[f, r] = parse({"a", "mod", "b", "mod", "c", "+"});

    EXPECT_FALSE(f);
    EXPECT_TRUE(r.empty());
}

TEST(ParserTest, Test6)
{
    auto &&[f, r] = parse({"a", "and", "c", "xor", "(", "b", "or", "c", ")", "and", "1"});

    EXPECT_TRUE(f);
    EXPECT_EQ(r, "a c and b c or xor 1 and");
}

TEST(ParserTest, Test7)
{
    auto &&[f, r] = parse({"+'", "b", "*", "(", "-\"", "c", ")"});

    EXPECT_TRUE(f);
    EXPECT_EQ(r, "b +' c -\" *");
}

TEST(ParserTest, Test8)
{
    auto &&[f, r] = parse({"(", "not", "a", "and", "b", ")", "and", "(", "c", "or", "d", ")"});

    EXPECT_TRUE(f);
    EXPECT_EQ(r, "a b and not c d or and");
}

TEST(ParserTest, Test9)
{
    auto &&[f, r] = parse({"(", "a", "*", "2", ")", "mod", "c", "+\"", "("});

    EXPECT_FALSE(f);
    EXPECT_TRUE(r.empty());
}

TEST(ParserTest, Test10)
{
    auto &&[f, r] = parse({"abs", "a", "*", "abs", "(", "b", "-\"", "c", ")"});

    EXPECT_TRUE(f);
    EXPECT_EQ(r, "a b c -\" abs * abs");
}

TEST(ParserTest, Test11)
{
    auto &&[f, r] = parse({"(", "a", "b", "*", "c", ")"});

    EXPECT_FALSE(f);
    EXPECT_TRUE(r.empty());
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
