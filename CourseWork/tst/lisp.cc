#include <gtest/gtest.h>

#include "lisp.h"

static std::shared_ptr<lisp::Environment> env = lisp::init();

#define TEST_EXPR(expr, expected_result, env) \
    EXPECT_EQ(lisp::toString(lisp::eval(lisp::read(expr), env)), expected_result)

TEST(LispTests, Test1)
{
    TEST_EXPR("(quote (testing 1 (2.0) -3.14e159))", "(testing 1 (2.0) -3.14e159)", env);
}

TEST(LispTests, Test2)
{
    TEST_EXPR("(+ 2 2)", "4", env);
}

TEST(LispTests, Test3)
{
    TEST_EXPR("(+ (* 2 100) (* 1 10))", "210", env);
}

TEST(LispTests, Test4)
{
    TEST_EXPR("(if (> 6 5) (+ 1 1) (+ 2 2))", "2", env);
}

TEST(LispTests, Test5)
{
    TEST_EXPR("(if (< 6 5) (+ 1 1) (+ 2 2))", "4", env);
}

TEST(LispTests, Test6)
{
    TEST_EXPR("(define x 3)", "3", env);
}

TEST(LispTests, Test7)
{
    TEST_EXPR("x", "3", env);
}

TEST(LispTests, Test8)
{
    TEST_EXPR("(+ x x)", "6", env);
}

TEST(LispTests, Test9)
{
    TEST_EXPR("(begin (define x 1) (set! x (+ x 1)) (+ x 1))", "3", env);
}

TEST(LispTests, Test10)
{
    TEST_EXPR("((lambda (x) (+ x x)) 5)", "10", env);
}

TEST(LispTests, Test11)
{
    TEST_EXPR("(define twice (lambda (x) (* 2 x)))", "<Lambda>", env);
}

TEST(LispTests, Test12)
{
    TEST_EXPR("(twice 5)", "10", env);
}

TEST(LispTests, Test13)
{
    TEST_EXPR("(define compose (lambda (f g) (lambda (x) (f (g x)))))", "<Lambda>", env);
}

TEST(LispTests, Test14)
{
    TEST_EXPR("((compose list twice) 5)", "(10)", env);
}

TEST(LispTests, Test15)
{
    TEST_EXPR("(define repeat (lambda (f) (compose f f)))", "<Lambda>", env);
}

TEST(LispTests, Test16)
{
    TEST_EXPR("((repeat twice) 5)", "20", env);
}

TEST(LispTests, Test17)
{
    TEST_EXPR("((repeat (repeat twice)) 5)", "80", env);
}

TEST(LispTests, Test18)
{
    TEST_EXPR("(define fact (lambda (n) (if (<= n 1) 1 (* n (fact (- n 1))))))", "<Lambda>", env);
}

TEST(LispTests, Test19)
{
    TEST_EXPR("(fact 3)", "6", env);
}

TEST(LispTests, Test20)
{
    TEST_EXPR("(fact 12)", "479001600", env);
}

TEST(LispTests, Test21)
{
    TEST_EXPR("(define abs (lambda (n) ((if (> n 0) + -) 0 n)))", "<Lambda>", env);
}

TEST(LispTests, Test22)
{
    TEST_EXPR("(list (abs -3) (abs 0) (abs 3))", "(3 0 3)", env);
}

TEST(LispTests, Test23)
{
    TEST_EXPR(
        "(define combine (lambda (f)"
        "(lambda (x y)"
        "(if (null? x) (quote ())"
        "(f (list (car x) (car y))"
        "((combine f) (cdr x) (cdr y)))))))",
        "<Lambda>", env);
}

TEST(LispTests, Test24)
{
    TEST_EXPR("(define zip (combine cons))", "<Lambda>", env);
}

TEST(LispTests, Test25)
{
    TEST_EXPR("(zip (list 1 2 3 4) (list 5 6 7 8))", "((1 5) (2 6) (3 7) (4 8))", env);
}

TEST(LispTests, Test26)
{
    TEST_EXPR(
        "(define riff-shuffle (lambda (deck) (begin"
        "(define take (lambda (n seq) (if (<= n 0) (quote ()) (cons (car seq) (take (- n 1) (cdr "
        "seq))))))"
        "(define drop (lambda (n seq) (if (<= n 0) seq (drop (- n 1) (cdr seq)))))"
        "(define mid (lambda (seq) (/ (length seq) 2)))"
        "((combine append) (take (mid deck) deck) (drop (mid deck) deck)))))",
        "<Lambda>", env);
}

TEST(LispTests, Test27)
{
    TEST_EXPR("(riff-shuffle (list 1 2 3 4 5 6 7 8))", "(1 5 2 6 3 7 4 8)", env);
}

TEST(LispTests, Test28)
{
    TEST_EXPR("((repeat riff-shuffle) (list 1 2 3 4 5 6 7 8))", "(1 3 5 7 2 4 6 8)", env);
}

TEST(LispTests, Test29)
{
    TEST_EXPR("(riff-shuffle (riff-shuffle (riff-shuffle (list 1 2 3 4 5 6 7 8))))",
        "(1 2 3 4 5 6 7 8)", env);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
