#pragma once
#include "Environments.h"
#include "Position.h"
#include "Symbol.h"

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <boost/optional.hpp>

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
    virtual const Position& UsePosition()
    {
        return m_position;
    }

    virtual void SetPosition(const Position& position)
    {
        m_position = position;
    }

    virtual void ReportError(std::string& message);

    virtual Type TypeCheck() = 0;
    
    virtual ~AstNode() {}
    
    AstNode()
        : m_position({0,0})
    {}

    void SetEnvironments(const std::shared_ptr<ValueEnvironment>& valEnv, const std::shared_ptr<TypeEnvironment>& tyEnv)
    {
        if (!valEnv || !tyEnv)
        {
            throw CompilerErrorException("Internal compiler error: Attempt to set internal environment to invalid value");
        }

        m_valueEnvironment = valEnv;
        m_typeEnvironment = tyEnv;
    }

    std::shared_ptr<ValueEnvironment>& UseValueEnvironment()
    {
        return m_valueEnvironment;
    }

    std::shared_ptr<TypeEnvironment>& UseTypeEnvironment()
    {
        return m_typeEnvironment;
    }

private:
    Position m_position;

    std::shared_ptr<ValueEnvironment> m_valueEnvironment;
    std::shared_ptr<TypeEnvironment> m_typeEnvironment;
};

struct Expression : public AstNode {};

struct Var : public AstNode {};

struct Declaration : public AstNode {};

struct TypeNode : public AstNode {};

class Program
{
public:
    Program(std::unique_ptr<Expression>&& expr, const std::shared_ptr<ValueEnvironment>& valEnv, const std::shared_ptr<TypeEnvironment>& typeEnv)
        : m_expression(std::move(expr))
        , m_valueEnvironment(valEnv)
        , m_typeEnvironment(typeEnv)
    {
    }

    Program(std::unique_ptr<Expression>&& expr)
        : Program(std::move(expr), ValueEnvironment::GenerateBaseValueEnvironment(), TypeEnvironment::GenerateBaseTypeEnvironment())
    {
    }

    const Expression& UseExpression()
    {
        return *m_expression;
    }

    Type TypeCheck();

private:
    std::unique_ptr<Expression> m_expression;
    std::shared_ptr<ValueEnvironment> m_valueEnvironment;
    std::shared_ptr<TypeEnvironment> m_typeEnvironment;
};

struct SimpleVar
    : public Var
{
    Symbol symbol;
    SimpleVar(const Symbol& sym)
        : symbol(sym) {}
    Type TypeCheck() override;
};

struct FieldVar
    : public Var
{
    Symbol symbol;
    std::unique_ptr<Var> var;
    FieldVar(const Symbol& sym, std::unique_ptr<Var>&& invar)
        : symbol(sym)
        , var(std::move(invar)) {}
    Type TypeCheck() override;
};

struct SubscriptVar
    : public Var
{
    std::unique_ptr<Var> var;
    std::unique_ptr<Expression> expression; 
    SubscriptVar(std::unique_ptr<Var>&& inVar, std::unique_ptr<Expression>&& exp)
        : var(std::move(inVar))
        , expression(std::move(exp)) {}
    Type TypeCheck() override;
};

struct VarExpression
    : public Expression
{
    std::unique_ptr<Var> var;
    VarExpression(std::unique_ptr<Var>&& inVar)
        : var(std::move(inVar)) {}
    Type TypeCheck() override;
};

struct NilExpression
    : public Expression
{ 
    Type TypeCheck() override;
};

struct IntExpression
    : public Expression
{
    int value;
    IntExpression(int val)
        : value(val) {}
    Type TypeCheck() override;
};

struct StringExpression
    : public Expression
{
    std::string value;
    StringExpression(const std::string& val)
        : value(val) {}
    Type TypeCheck() override;
};

struct CallExpression
    : public Expression
{
    Symbol function;
    std::vector<std::unique_ptr<Expression>> args;
    CallExpression(const Symbol& fn, std::vector<std::unique_ptr<Expression>>&& ars)
        : function(fn)
        , args(std::move(ars)) {}
    Type TypeCheck() override;
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
    Type TypeCheck() override;
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
    Type TypeCheck() override;
};

struct SeqExpression
    : public Expression
{
    std::vector<std::unique_ptr<Expression>> expressions;
    SeqExpression(std::vector<std::unique_ptr<Expression>>&& expr)
        : expressions(std::move(expr)) {}
    Type TypeCheck() override;
};

struct AssignmentExpression
    : public Expression
{
    std::unique_ptr<Var> var;
    std::unique_ptr<Expression> expression;
    AssignmentExpression(std::unique_ptr<Var>&& v, std::unique_ptr<Expression> expr)
        : var(std::move(v))
        , expression(std::move(expr)) {}
    Type TypeCheck() override;
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
    Type TypeCheck() override;
};

struct WhileExpression
    : public Expression
{
    std::unique_ptr<Expression> test;
    std::unique_ptr<Expression> body;

    WhileExpression(std::unique_ptr<Expression>&& t, std::unique_ptr<Expression>&& b)
        : test(std::move(t))
        , body(std::move(b)) {}
    Type TypeCheck() override;
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
    Type TypeCheck() override;
};

struct BreakExpression
    : public Expression
{
    Type TypeCheck() override;
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
    Type TypeCheck() override;
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
    Type TypeCheck() override;
};

struct FunDec
{
    Symbol name;
    std::vector<Field> fields;
    boost::optional<Symbol> resultTy;
    std::unique_ptr<Expression> body;

    FunDec(const Symbol& nam, std::vector<Field>&& flds, boost::optional<Symbol> ty, std::unique_ptr<Expression>&& bdy)
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
    Type TypeCheck() override;
};

struct VarDeclaration
    : public Declaration
{
   Symbol name;
   bool escape;
   boost::optional<Symbol> type;
   std::unique_ptr<Expression> init;

   VarDeclaration(const Symbol& id, boost::optional<Symbol> ty, std::unique_ptr<Expression>&& it)
    : name(id)
    , escape(true)
    , type(ty)
    , init(std::move(it))
   {}
    Type TypeCheck() override;
};

struct TyDec
{
    Symbol name;
    std::unique_ptr<TypeNode> type;
    TyDec(const Symbol& id, std::unique_ptr<TypeNode>&& ty)
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
    Type TypeCheck() override;
};

struct NameType
    : public TypeNode
{
    Symbol name;

    NameType(const Symbol& id)
        : name(id) {}
    Type TypeCheck() override;
};

struct RecordType
    : public TypeNode
{
    std::vector<Field> fields;

    RecordType(std::vector<Field>&& flds)
        : fields(std::move(flds)) {}
    Type TypeCheck() override;
};

struct ArrayType
    : public TypeNode
{
    Symbol name;

    ArrayType(const Symbol& id)
        : name(id) {}
    Type TypeCheck() override;
};

}
