#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <numeric>

enum class CellType
{
    Symbol,
    Number,
    List,
    Proc,
    Lambda
};

struct Environment;  // forward declaration; Cell and Environment reference each other

// a variant that can hold any kind of lisp value
struct Cell
{
    CellType type;

    std::string val;
    std::vector<Cell> list;
    std::function<Cell(const std::vector<Cell> &)> proc;

    std::shared_ptr<Environment> env;

    Cell(CellType type = CellType::Symbol) : type(type) {}
    Cell(CellType type, const std::string &val) : type(type), val(val) {}
    Cell(std::function<Cell(const std::vector<Cell> &)> proc) : type(CellType::Proc), proc(proc) {}
};

static const Cell False(CellType::Symbol, "#f");
static const Cell True(CellType::Symbol, "#t");  // anything that isn't false_sym is true
static const Cell Nil(CellType::Symbol, "nil");

// a dictionary that (a) associates symbols with std::vector<Cell>, and
// (b) can chain to an "outer" dictionary
struct Environment
{
    Environment() = default;
    Environment(const std::vector<Cell> &parms,
        const std::vector<Cell> &args,
        std::weak_ptr<Environment> outer)
        : outer_(outer)
    {
        auto a = args.begin();
        for (auto p = parms.begin(); p != parms.end(); ++p)
            env_[p->val] = *a++;
    }

    std::map<std::string, Cell> &find(const std::string &var)
    {
        if (env_.find(var) != env_.end())
        {
            return env_;  // the symbol exists in this Environment
        }

        if (auto ptr = outer_.lock())
        {
            return ptr->find(var);  // attempt to find the symbol in some "outer" env
        }

        throw std::runtime_error("Unbound symbol '" + var + "\n");
    }

    // return a reference to the Cell associated with the given symbol 'var'
    Cell &operator[](const std::string &var)
    {
        return env_[var];
    }

private:
    std::map<std::string, Cell> env_;  // inner symbol->Cell mapping
    std::weak_ptr<Environment>
        outer_;  // next adjacent outer env, or 0 if there are no further Environments
};

Cell procAdd(std::vector<Cell> cells)
{
    auto result = std::accumulate(std::next(std::begin(cells)), std::end(cells),
        std::stoll(cells.front().val), [](auto x, auto &&y) { return x + std::stoll(y.val); });
    return Cell(CellType::Number, std::to_string(result));
}

Cell procSub(std::vector<Cell> cells)
{
    auto result = std::accumulate(std::next(std::begin(cells)), std::end(cells),
        std::stoll(cells.front().val), [](auto x, auto &&y) { return x - std::stoll(y.val); });
    return Cell(CellType::Number, std::to_string(result));
}

Cell procMul(std::vector<Cell> cells)
{
    auto result = std::accumulate(std::begin(cells), std::end(cells), static_cast<long long>(1),
        [](auto x, auto &&y) { return x * std::stoll(y.val); });
    return Cell(CellType::Number, std::to_string(result));
}

Cell procDiv(std::vector<Cell> cells)
{
    auto result = std::accumulate(std::next(std::begin(cells)), std::end(cells),
        std::stoll(cells.front().val), [](auto x, auto &&y) { return x / std::stoll(y.val); });
    return Cell(CellType::Number, std::to_string(result));
}

Cell procGreater(std::vector<Cell> cells)
{
    auto first = std::stoll(cells.front().val);
    if (std::find_if(std::next(std::begin(cells)), std::end(cells),
            [&](auto &&x) { return first <= std::stoll(x.val); }) != std::end(cells))
    {
        return False;
    }
    return True;
}

Cell procLess(std::vector<Cell> cells)
{
    auto first = std::stoll(cells.front().val);
    if (std::find_if(std::next(std::begin(cells)), std::end(cells),
            [&](auto &&x) { return first >= std::stoll(x.val); }) != std::end(cells))
    {
        return False;
    }
    return True;
}

Cell procLessEqual(std::vector<Cell> cells)
{
    auto first = std::stoll(cells.front().val);
    if (std::find_if(std::next(std::begin(cells)), std::end(cells),
            [&](auto &&x) { return first > std::stoll(x.val); }) != std::end(cells))
    {
        return False;
    }
    return True;
}

Cell procLength(std::vector<Cell> cells)
{
    return Cell(CellType::Number, std::to_string(cells.front().list.size()));
}
Cell procNullp(std::vector<Cell> cells)
{
    return cells.front().list.empty() ? True : False;
}
Cell procCar(std::vector<Cell> cells)
{
    return cells.front().list.front();
}

Cell procCdr(std::vector<Cell> cells)
{
    if (cells.front().list.size() < 2)
    {
        return Nil;
    }

    Cell result(cells.front());
    result.list.erase(result.list.begin());
    return result;
}

Cell procAppend(std::vector<Cell> cells)
{
    Cell result(CellType::List);
    result.list = cells.front().list;
    for (auto &&i: cells[1].list)
    {
        result.list.push_back(i);
    }
    return result;
}

Cell procCons(std::vector<Cell> cells)
{
    Cell result(CellType::List);
    result.list.push_back(cells.front());
    for (auto &&i: cells[1].list)
    {
        result.list.push_back(i);
    }
    return result;
}

Cell procList(std::vector<Cell> cells)
{
    Cell result(CellType::List);
    result.list = cells;
    return result;
}

std::shared_ptr<Environment> init()
{
    Environment env;
    env["nil"] = Nil;
    env["#f"] = False;
    env["#t"] = True;
    env["append"] = Cell(procAppend);
    env["car"] = Cell(procCar);
    env["cdr"] = Cell(procCdr);
    env["cons"] = Cell(procCons);
    env["length"] = Cell(procLength);
    env["list"] = Cell(procList);
    env["null?"] = Cell(procNullp);
    env["+"] = Cell(procAdd);
    env["-"] = Cell(procSub);
    env["*"] = Cell(procMul);
    env["/"] = Cell(procDiv);
    env[">"] = Cell(procGreater);
    env["<"] = Cell(procLess);
    env["<="] = Cell(procLessEqual);
    return std::make_shared<Environment>(env);
}

Cell eval(Cell x, std::shared_ptr<Environment> env)
{
    if (x.type == CellType::Symbol)
    {
        return env->find(x.val)[x.val];
    }
    if (x.type == CellType::Number)
    {
        return x;
    }
    if (x.list.empty())
    {
        return Nil;
    }
    if (x.list[0].type == CellType::Symbol)
    {
        if (x.list[0].val == "quote")  // (quote exp)
        {
            return x.list[1];
        }
        if (x.list[0].val == "if")  // (if test conseq [alt])
        {
            return eval(eval(x.list[1], env).val == "#f" ? (x.list.size() < 4 ? Nil : x.list[3])
                                                         : x.list[2],
                env);
        }
        if (x.list[0].val == "set!")  // (set! var exp)
        {
            return env->find(x.list[1].val)[x.list[1].val] = eval(x.list[2], env);
        }
        if (x.list[0].val == "define")  // (define var exp)
        {
            return (*env)[x.list[1].val] = eval(x.list[2], env);
        }
        if (x.list[0].val == "lambda")  // (lambda (var*) exp)
        {
            x.type = CellType::Lambda;
            x.env = env;
            return x;
        }
        if (x.list[0].val == "begin")
        {  // (begin exp*)
            for (size_t i = 1; i < x.list.size() - 1; ++i)
            {
                eval(x.list[i], env);
            }
            return eval(x.list[x.list.size() - 1], env);
        }
    }
    // (proc exp*)
    Cell proc(eval(x.list[0], env));
    std::vector<Cell> exps;
    for (auto exp = x.list.begin() + 1; exp != x.list.end(); ++exp)
    {
        exps.push_back(eval(*exp, env));
    }

    if (proc.type == CellType::Lambda)
    {
        return eval(/*body*/ proc.list[2],
            std::make_shared<Environment>(/*parms*/ proc.list[1].list, /*args*/ exps, proc.env));
    }
    else if (proc.type == CellType::Proc)
    {
        return proc.proc(exps);
    }

    throw std::runtime_error("Not a function!");
}

// convert given string to list of tokens
std::list<std::string> tokenize(std::string_view str)
{
    std::list<std::string> tokens;
    const char *s = str.data();

    while (*s)
    {
        while (std::isspace(*s))
        {
            ++s;
        }

        if (*s == '(' || *s == ')')
        {
            tokens.push_back(*s++ == '(' ? "(" : ")");
        }
        else
        {
            const char *t = s;
            while (*t && *t != ' ' && *t != '(' && *t != ')')
            {
                ++t;
            }

            tokens.push_back(std::string(s, t));
            s = t;
        }
    }
    return tokens;
}

// numbers become Numbers; every other token is a Symbol
Cell atom(std::string token)
{
    if (std::isdigit(token[0]) || (token[0] == '-' && std::isdigit(token[1])))
    {
        return Cell(CellType::Number, token);
    }
    return Cell(CellType::Symbol, token);
}

// return the Lisp expression in the given tokens
Cell readFrom(std::list<std::string> &tokens)
{
    if (!tokens.empty())
    {
        const std::string token(tokens.front());
        tokens.pop_front();
        if (token == "(")
        {
            Cell cell(CellType::List);
            while (tokens.front() != ")")
            {
                cell.list.push_back(readFrom(tokens));
            }

            tokens.pop_front();
            return cell;
        }
        else
        {
            return atom(std::move(token));
        }
    }
    return Nil;
}

// return the Lisp expression represented by the given string
Cell read(std::string_view str)
{
    auto tokens = tokenize(str);
    return readFrom(tokens);
}

// convert given Cell to a Lisp-readable string
std::string to_string(Cell exp)
{
    switch (exp.type)
    {
        case CellType::List:
        {
            std::string str("(");
            std::string space;

            for (auto &&e: exp.list)
            {
                str += space + to_string(e);
                space = " ";
            }
            return str + ')';
        }
        case CellType::Lambda:
            return "<Lambda>";
        case CellType::Proc:
            return "<Proc>";
        default:
            return exp.val;
    }
}

// the default read-eval-print-loop
void repl(std::istream &input,
    std::ostream &output,
    std::string_view prompt,
    std::shared_ptr<Environment> env)
{
    for (;;)
    {
        output << prompt;
        std::string line;
        std::getline(input, line);

        try
        {
            output << to_string(eval(read(line), env)) << '\n';
        }
        catch (const std::runtime_error &err)
        {
            output << err.what();
        }
    }
}

int main()
{
    repl(std::cin, std::cout, "lispy> ", init());
    return 0;
}
