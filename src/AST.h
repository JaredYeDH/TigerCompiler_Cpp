#pragma once
#include <memory>
#include <string>
#include <vector>
#include <iostream>

typedef std::string Symbol;

namespace AST
{

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

struct Field
{
    Symbol name;
    bool escape;
    Symbol type;

    Field(const Symbol& nm, const Symbol& ty)
        : name(nm)
        , escape(true)
        , type(ty)
    {}
};

struct AstNode
{
    virtual ~AstNode(){}
};

struct Var : public AstNode 
{
};

struct Expression : public AstNode
{
};

struct Declaration : public AstNode
{
};

struct Type : public AstNode
{
};

// While a Tiger program is just an expression, 
// it is useful to have this here in case we
// want to support top level decls or something 
// to similar.
struct Program
{
    std::unique_ptr<Expression> expression;
    Program(std::unique_ptr<Expression>&& expr)
        : expression(std::move(expr)) { /* empty */ }
};

struct SimpleVar
    : public Var
{
    Symbol symbol;
    SimpleVar(const Symbol& sym)
        : symbol(sym) {}
};

struct FieldVar
    : public Var
{
    Symbol symbol;
    std::unique_ptr<Var> var;
    FieldVar(const Symbol& sym, std::unique_ptr<Var>&& invar)
        : symbol(sym)
        , var(std::move(invar)) {}
};

struct SubscriptVar
    : public Var
{
    std::unique_ptr<Var> var;
    std::unique_ptr<Expression> expression; 
    SubscriptVar(std::unique_ptr<Var>&& inVar, std::unique_ptr<Expression>&& exp)
        : var(std::move(inVar))
        , expression(std::move(exp)) {}
};

struct VarExpression
    : public Expression
{
    std::unique_ptr<Var> var;
    VarExpression(std::unique_ptr<Var>&& inVar)
        : var(std::move(inVar)) {}
};

struct NilExpression
    : public Expression
{
};

struct IntExpression
    : public Expression
{
    int value;
    IntExpression(int val)
        : value(val) {}
};

struct StringExpression
    : public Expression
{
    std::string value;
    StringExpression(const std::string& val)
        : value(val) {}
};

struct CallExpression
    : public Expression
{
    Symbol function;
    std::vector<std::unique_ptr<Expression>> args;
    CallExpression(const Symbol& fn, std::vector<std::unique_ptr<Expression>>&& ars)
        : function(fn)
        , args(std::move(ars)) {}
};

struct OpExpression
    : public Expression
{
    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;
    BinOp op;
    OpExpression(std::unique_ptr<Expression>&& ls, std::unique_ptr<Expression>&& rs, BinOp oper)
        : lhs(std::move(ls))
        , rhs(std::move(rs))
        , op(oper) {}
};

struct FieldExp
{
    Symbol field;
    std::unique_ptr<Expression> expr;

    FieldExp(const Symbol& fld, std::unique_ptr<Expression>&& ex)
        : field(fld)
        , expr(std::move(ex))
    {}
};

struct RecordExpression
    : public Expression
{
    Symbol type;
    std::vector<FieldExp> fields;
    RecordExpression(const Symbol& ty, std::vector<FieldExp>&& flds)
        : type(ty)
        , fields(std::move(flds)) {}
};

struct SeqExpression
    : public Expression
{
    std::vector<std::unique_ptr<Expression>> expressions;
    SeqExpression(std::vector<std::unique_ptr<Expression>>&& expr)
        : expressions(std::move(expr)) {}
};

struct AssignmentExpression
    : public Expression
{
    std::unique_ptr<Var> var;
    std::unique_ptr<Expression> expression;
    AssignmentExpression(std::unique_ptr<Var>&& v, std::unique_ptr<Expression> expr)
        : var(std::move(v))
        , expression(std::move(expr)) {}
};

struct IfExpression
    : public Expression
{
    std::unique_ptr<Expression> test;
    std::unique_ptr<Expression> thenBranch;
    std::unique_ptr<Expression> elseBranch;

    IfExpression(
            std::unique_ptr<Expression>&& t,
            std::unique_ptr<Expression>&& thn,
            std::unique_ptr<Expression>&& els)
        : test(std::move(t))
        , thenBranch(std::move(thn))
        , elseBranch(std::move(els))
    {}
};

struct WhileExpression
    : public Expression
{
    std::unique_ptr<Expression> test;
    std::unique_ptr<Expression> body;

    WhileExpression(std::unique_ptr<Expression>&& t, std::unique_ptr<Expression>&& b)
        : test(std::move(t))
        , body(std::move(b)) {}
};

struct ForExpression
    : public Expression
{
    Symbol var;
    bool escape;
    std::unique_ptr<Expression> low;
    std::unique_ptr<Expression> high;
    std::unique_ptr<Expression> body;

    ForExpression(const Symbol& v, std::unique_ptr<Expression>&& l, std::unique_ptr<Expression>&& h, std::unique_ptr<Expression>&& b)
        : var(v)
        , escape(true)
        , low(move(l))
        , high(move(h))
        , body(move(b)) {}
};

struct BreakExpression
    : public Expression
{
};

struct LetExpression
    : public Expression
{
    std::vector<std::unique_ptr<Declaration>> decls;
    std::unique_ptr<Expression> body;

    LetExpression(std::vector<std::unique_ptr<Declaration>>&& decs, std::unique_ptr<Expression>&& bdy)
        : decls(std::move(decs))
        , body(std::move(bdy))
        {}
};

struct ArrayExpression
    : public Expression
{
    Symbol type;
    std::unique_ptr<Expression> size;
    std::unique_ptr<Expression> init;

    ArrayExpression(const Symbol& id, std::unique_ptr<Expression>&& sz, std::unique_ptr<Expression>&& val)
        : type(id)
        , size(std::move(sz))
        , init(std::move(val))
        {}
};

struct FunDec
{
    Symbol name;
    std::vector<Field> fields;
    Symbol resultTy;
    std::unique_ptr<Expression> body;

    FunDec(const Symbol& nam, std::vector<Field>&& flds, const Symbol& ty, std::unique_ptr<Expression>&& bdy)
        : name(nam)
        , fields(std::move(flds))
        , resultTy(ty)
        , body(std::move(bdy))
    {}
};

struct FunctionDeclaration
    : public Declaration
{
    std::vector<FunDec> decls;

    FunctionDeclaration(std::vector<FunDec>&& decs)
        : decls(std::move(decs)) {}
};

struct VarDeclaration
    : public Declaration
{
   Symbol name;
   bool escape;
   Symbol type;
   std::unique_ptr<Expression> init;

   VarDeclaration(const Symbol& id, const Symbol& ty, std::unique_ptr<Expression>&& it)
    : name(id)
    , escape(true)
    , type(ty)
    , init(std::move(it))
   {}
};

struct TyDec
{
    Symbol name;
    std::unique_ptr<Type> type;
    TyDec(const Symbol& id, std::unique_ptr<Type>&& ty)
        : name(id)
        , type(std::move(ty))
    {}
};

struct TypeDeclaration
    : public Declaration
{
    std::vector<TyDec> types;

    TypeDeclaration(std::vector<TyDec>&& ty)
        : types(std::move(ty))
    {}
};

struct NameType
    : public Type
{
    Symbol name;

    NameType(const Symbol& id)
        : name(id) {}
};

struct RecordType
    : public Type
{
    std::vector<Field> fields;

    RecordType(std::vector<Field>&& flds)
        : fields(std::move(flds)) {}
};

struct ArrayType
    : public Type
{
    Symbol name;

    ArrayType(const Symbol& id)
        : name(id) {}
};

}
