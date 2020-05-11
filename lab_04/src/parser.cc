#include "parser.h"

#include <map>
#include <set>

static const std::string MARKER = "$";
static const std::vector<std::vector<std::string>> operators = {
    {"**", "abs", "not"},
    {"*", "/", "mod", "rem"},
    {"+'", "-'"},
    {"+\"", "-\"", "&"},
    {"<", "<=", "=", "/>", ">", ">="},
    {"and", "or", "xor"},
};
static const std::set<std::string> prefixes = {"abs", "not", "+'", "-'"};
static const std::set<std::string> identifiers = {"a", "b", "c", "d"};
static const std::set<std::string> constants = {"0", "1", "2"};

static const auto fillPrecedence()
{
    std::map<std::string, int> precedence;
    for (int i = static_cast<int>(operators.size()) - 1; i >= 0; --i)
    {
        for (auto &&op: operators[i])
        {
            precedence[op] = i;
        }
    }
    return precedence;
}

static const auto getAllTokens()
{
    std::set<std::string> allTokens;
    for (auto &&ops: operators)
    {
        std::copy(std::begin(ops), std::end(ops), std::inserter(allTokens, std::end(allTokens)));
    }
    std::copy(
        std::begin(prefixes), std::end(prefixes), std::inserter(allTokens, std::end(allTokens)));
    std::copy(std::begin(identifiers), std::end(identifiers),
        std::inserter(allTokens, std::end(allTokens)));
    std::copy(
        std::begin(constants), std::end(constants), std::inserter(allTokens, std::end(allTokens)));
    allTokens.insert({"(", ")", MARKER});
    return allTokens;
}

static const auto makeRelations()
{
    static const std::set<std::string> rightAssociative = {"**"};
    std::map<std::string, std::map<std::string, std::string>> relations = {
        {MARKER, {{"(", "<"}}},
        {"(", {{")", "="}, {"(", "<"}}},
        {")", {{"$", ">"}, {")", ">"}}},
    };

    std::vector<std::string> variables;
    std::copy(std::begin(identifiers), std::end(identifiers), std::back_inserter(variables));
    std::copy(std::begin(constants), std::end(constants), std::back_inserter(variables));

    for (auto &&var: variables)
    {
        relations[MARKER][var] = relations["("][var] = "<";
        relations[var][MARKER] = relations[var][")"] = ">";
    }

    // Here 'relations[op]' can be stored to speedup
    auto precedence = fillPrecedence();
    for (auto &&[op, _]: precedence)
    {
        relations[op][MARKER] = ">";
        relations[MARKER][op] = "<";

        relations[op]["("] = relations["("][op] = "<";
        relations[op][")"] = relations[")"][op] = ">";

        for (auto &&var: variables)
        {
            relations[op][var] = "<";
            relations[var][op] = ">";
        }

        if (prefixes.find(op) != std::end(prefixes))
        {
            for (auto &&[op_, pr_]: precedence)
            {
                relations[op_][op] = "<";
                relations[op][op_] = precedence[op] > precedence[op_] ? ">" : "<";
            }
        }
        else
        {
            for (auto &&[op_, _]: precedence)
            {
                auto prOp = precedence[op];
                auto prOp_ = precedence[op_];

                auto isRightAssociativeOp = rightAssociative.find(op) != std::end(rightAssociative);
                auto isRightAssociativeOp_ =
                    rightAssociative.find(op_) != std::end(rightAssociative);

                if (prOp < prOp_ ||
                    (prOp == prOp_ && isRightAssociativeOp && isRightAssociativeOp_))
                {
                    relations[op][op_] = "<";
                }
                else if (prOp > prOp_ ||
                         (prOp == prOp_ && !isRightAssociativeOp && !isRightAssociativeOp_))
                {
                    relations[op][op_] = ">";
                }
            }
        }
    }

    return relations;
}

std::pair<bool, std::string> parse(std::vector<std::string> tokens)
{
    auto allTokens = getAllTokens();
    auto relations = makeRelations();
    tokens.push_back(MARKER);

    std::string result;
    auto nextToken = std::begin(tokens);
    std::vector<std::string> stackTail;
    auto stackHead = MARKER;

    do
    {
        if (allTokens.find(*nextToken) != std::end(allTokens))
        {
            auto &&relation = relations[stackHead][*nextToken];
            if (relation == "<" || relation == "=")
            {
                stackTail.push_back(stackHead);
                stackHead = *nextToken;
                ++nextToken;
            }
            else if (relation == ">")
            {
                std::string oldStackHead;
                do
                {
                    if (stackHead != "(" && stackHead != ")")
                    {
                        result.append(stackHead);
                        result.push_back(' ');
                    }

                    oldStackHead = stackHead;
                    stackHead = stackTail.back();
                    stackTail.pop_back();
                } while (relations[stackHead][oldStackHead] != "<");
            }
            else
            {
                return {false, {}};
            }
        }
        else
        {
            return {false, {}};
        }
    } while (stackHead != MARKER || *nextToken != MARKER);

    result.pop_back();
    return {true, result};
}
