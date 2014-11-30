#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace AST
{
typedef std::string Symbol;

enum class BinOp : unsigned int
{
   Plus,
   Minus,
   Times,
   Div,
   Eq,
   Neq,
   Lt,
   Le,
   Gt,
   Ge 
};

struct Feild
{
    Symbol name;
    bool escape;
    Symbol type;
};

struct FunDec
{
    Symbol name;
    std::vector<Feild> fields;
    Symbol result;
};

struct Var {};
struct Expression {};
struct Declaration {};
struct Type {};

struct SimpleVar
    : public Var
{
    Symbol symbol;
};

struct FieldVar
    : public Var
{
    Symbol symbol;
    std::unique_ptr<Var> var;
};

struct SubscriptVar
    : public Var
{
    std::unique_ptr<Var> var;
    std::unique_ptr<Expression> expression; 
};

struct VarExpression
    : public Expression
{
    std::unique_ptr<Var> var;
};

struct NilExpression
    : public Expression
{
};

struct IntExpression
    : public Expression
{
    int value;
};

struct StringExpression
    : public Expression
{
    std::string value;
};

struct CallExpression
    : public Expression
{
    Symbol function;
    std::vector<std::unique_ptr<Expression>> args;
};

struct OpExpression
    : public Expression
{
    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;
    BinOp op;
};

struct RecordExpression
    : public Expression
{
    Symbol type;
    std::unordered_map<Symbol, std::unique_ptr<Expression>> fields;
};

struct SeqExpression
    : public Expression
{
    std::vector<std::unique_ptr<Expression>> expressions;
};

struct AssignmentExpression
    : public Expression
{
    std::unique_ptr<Var> var;
    std::unique_ptr<Expression> expression;
};

struct IfExpression
    : public Expression
{
    std::unique_ptr<Expression> test;
    std::unique_ptr<Expression> thenBranch;
    std::unique_ptr<Expression> elseBranch;
};

struct WhileExpression
    : public Expression
{
    std::unique_ptr<Expression> test;
    std::unique_ptr<Expression> body;
};

struct ForExpression
    : public Expression
{
    std::unique_ptr<Symbol> var;
    bool escape;
    std::unique_ptr<Expression> low;
    std::unique_ptr<Expression> high;
    std::unique_ptr<Expression> body;
};

struct BreakExpression
    : public Expression
{
};

struct LetExpression
    : public Expression
{
    std::vector<Declaration> decls;
    std::unique_ptr<Expression> body;
};

struct ArrayExpression
    : public Expression
{
    Symbol type;
    std::unique_ptr<Expression> size;
    std::unique_ptr<Expression> init;
};

struct FunctionDeclaration
    : public Declaration
{
    std::vector<FunDec> v;
};

struct VarDeclaration
    : public Declaration
{
   Symbol name;
   bool escape;
   Symbol type;
   std::unique_ptr<Expression> init; 
};

struct TypeDeclaration
    : public Declaration
{
    Symbol name;
    std::unique_ptr<Type> type;
};

struct NameType
    : public Type
{
    Symbol name;
};

struct RecordType
    : public Type
{
    std::vector<Feild> fields;
};

struct ArrayType
    : public Type
{
    Symbol name;
};

}
