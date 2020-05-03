#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "parser.h"

TEST(ParserTest, Test1)
{
    auto &&[tree, str] = parser::accept("{a=const}");

    EXPECT_TRUE(tree);
    EXPECT_TRUE(str.empty());
}

TEST(ParserTest, Test2)
{
    auto &&[tree, str] = parser::accept("{a=const;}");

    EXPECT_TRUE(!tree);
    EXPECT_TRUE(!str.empty());
}

TEST(ParserTest, Test3)
{
    auto &&[tree, str] = parser::accept("{a=const;a=b*a}");

    EXPECT_TRUE(tree);
    EXPECT_TRUE(str.empty());
}

TEST(ParserTest, Test4)
{
    auto &&[tree, str] = parser::accept("{a=const;{a=(b)*a}}");

    EXPECT_TRUE(tree);
    EXPECT_TRUE(str.empty());
}

TEST(ParserTest, Test5)
{
    auto &&[tree, str] = parser::accept("{a=const;{a=-a}}");

    EXPECT_TRUE(tree);
    EXPECT_TRUE(str.empty());
}

TEST(ParserTest, Test6)
{
    auto &&[tree, str] = parser::accept("{a=const<>a-a}");

    EXPECT_TRUE(tree);
    EXPECT_TRUE(str.empty());
}

TEST(ParserTest, Test7)
{
    auto &&[tree, str] = parser::accept("{a=const<>a;{b=notconst}}");

    EXPECT_TRUE(tree);
    EXPECT_TRUE(str.empty());
}

TEST(ParserTest, Test8)
{
    auto &&[tree, str] = parser::accept("{a = a div b mod c}");

    EXPECT_TRUE(tree);
    EXPECT_TRUE(str.empty());
}

TEST(ParserTest, Test9)
{
    auto &&[tree, str] = parser::accept("{a = a div b mod c; {a = +b <= const}}");

    EXPECT_TRUE(tree);
    EXPECT_TRUE(str.empty());
}

TEST(ParserTest, Test10)
{
    auto &&[tree, str] = parser::accept("{}");

    EXPECT_TRUE(!tree);
    EXPECT_TRUE(!str.empty());
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
