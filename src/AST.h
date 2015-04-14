#pragma once
#include "Environments.h"
#include "Position.h"
#include "Symbol.h"
#include "CompileTimeErrorHandler.h"
#include "EscapeCalculator.h"

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

inline const char* DumpOp(BinOp op)
{
    switch(op)
    {
    case (BinOp::Plus):
        return " + ";
    case (BinOp::Minus):
        return " - ";
    case (BinOp::Times):
        return " * ";
    case (BinOp::Div):
        return " / ";
    case (BinOp::Eq):
        return " = ";
    case (BinOp::Neq):
        return " <> ";
    case (BinOp::Lt):
        return " < ";
    case (BinOp::Le):
        return " <= ";
    case (BinOp::Gt):
        return " > ";
    case (BinOp::Ge):
        return " >= ";
    }
    return "<BustedOp>";
}

struct Field
{
    Symbol name;
    bool escape = false;
    Symbol type;
    Position position;

    Field(const Symbol& nm, const Symbol& ty, const Position& pos)
        : name(nm)
        , type(ty)
        , position(pos)
    {
    }

    std::string DumpAST() const
    {
        std::string escapeStr = escape ? " escapes" : " does not escape";
        return "{Field: {" + name.UseName() + "," + type.UseName() +  escapeStr.c_str() + "}}";
    }
};

struct AstNode
{
    virtual const Position& UsePosition()
    {
        return m_position;
    }

    virtual std::string DumpAST() const = 0;

    virtual Type TypeCheck() = 0;
    
    virtual ~AstNode() {}
    
    virtual void CalculateEscapes() = 0;
    
    void SetStaticErrorReporters(const std::shared_ptr<CompileTimeErrorReporter>& errReporter, const std::shared_ptr<WarningReporter>& warningReporter);

        void SetStaticEscapeCalculator(const std::shared_ptr<IEscapeCalculator>& escapes);

    void SetEnvironments(const std::shared_ptr<ValueEnvironment>& valEnv, const std::shared_ptr<TypeEnvironment>& tyEnv)
    {
        if (!valEnv || !tyEnv)
        {
            throw CompilerErrorException("Attempt to set internal environment to invalid value");
        }

        m_valueEnvironment = valEnv;
        m_typeEnvironment = tyEnv;
    }


    AstNode()
        : m_position({0,0})
    {}

protected:
   virtual const std::shared_ptr<ValueEnvironment>& UseValueEnvironment()
    {
        return m_valueEnvironment;
    }

    virtual const std::shared_ptr<TypeEnvironment>& UseTypeEnvironment()
    {
        return m_typeEnvironment;
    }

    virtual const std::shared_ptr<ValueEnvironment>& UseValueEnvironment() const
    {
        return m_valueEnvironment;
    }

    virtual const std::shared_ptr<IEscapeCalculator>& UseEscapeCalculator() const
    {
        return m_escapecalc;
    }

    virtual std::shared_ptr<CompileTimeErrorReporter>& UseErrorReporter();

    virtual std::shared_ptr<WarningReporter>& UseWarningReporter();

    virtual void ReportTypeError(ErrorCode errorCode, const SupplementalErrorMsg& message = "");

    virtual bool InLoopScope() const;

    virtual void EnterLoopScope();
    virtual void ExitLoopScope();

    virtual void SetPosition(const Position& position)
    {
        m_position = position;
    }

private:
    Position m_position;

    std::shared_ptr<ValueEnvironment> m_valueEnvironment;
    std::shared_ptr<TypeEnvironment> m_typeEnvironment;

    // TODO: this is hacky. These should be wired up correctly, not static.
    static std::shared_ptr<CompileTimeErrorReporter> m_errorReporter;
    static std::shared_ptr<WarningReporter> m_warningReporter;
    static uint8_t m_loopScope;
    static std::shared_ptr<IEscapeCalculator> m_escapecalc;
};

struct Expression : public AstNode {};

struct Var : public AstNode
{
    virtual bool IsImmutable() const = 0;
};

struct Declaration : public AstNode {};

struct TypeNode : public AstNode {};

class Program
{
public:
    Program(
            std::unique_ptr<Expression>&& expr,
            const std::shared_ptr<ValueEnvironment>& valEnv,
            const std::shared_ptr<TypeEnvironment>& typeEnv,
            const std::shared_ptr<CompileTimeErrorReporter>& errorReporter,
            const std::shared_ptr<WarningReporter>& warningReporter,
            std::shared_ptr<IEscapeCalculator> escapeCalc = nullptr)
        : m_expression(std::move(expr))
        , m_valueEnvironment(valEnv)
        , m_typeEnvironment(typeEnv)
        , m_errorReporter(errorReporter)
        , m_warningReporter(warningReporter)
    {
        m_expression->SetStaticErrorReporters(errorReporter, warningReporter);
        if (!escapeCalc)
        {
            escapeCalc = EscapeCalculatorFactory::MakeEscapeCalculator();
        }
        m_expression->SetStaticEscapeCalculator(escapeCalc);
    }

    Program(
            std::unique_ptr<Expression>&& expr,
            const std::shared_ptr<CompileTimeErrorReporter>& errorReporter,
            const std::shared_ptr<WarningReporter>& warningReporter)
        : Program(std::move(expr)
        , ValueEnvironment::GenerateBaseValueEnvironment()
        , TypeEnvironment::GenerateBaseTypeEnvironment()
        , errorReporter
        , warningReporter)
    {
    }

    std::string DumpAST() const
    {
        return m_expression->DumpAST();
    }

    const Expression& UseExpression()
    {
        return *m_expression;
    }

    Type TypeCheck();

    void CalculateEscapes()
    {
        m_expression->CalculateEscapes();
    }

private:
    std::unique_ptr<Expression> m_expression;
    std::shared_ptr<ValueEnvironment> m_valueEnvironment;
    std::shared_ptr<TypeEnvironment> m_typeEnvironment;
    std::shared_ptr<CompileTimeErrorReporter> m_errorReporter;
    std::shared_ptr<WarningReporter> m_warningReporter;
};

struct SimpleVar
    : public Var
{
    Symbol symbol;
    SimpleVar(const Symbol& sym, const Position& pos)
        : symbol(sym) 
    {
        SetPosition(pos);
    }
    
    std::string DumpAST() const override
    {
        return "{SimpleVar: " + symbol.UseName() + "}";
    }

    bool IsImmutable() const override
    {
        bool isImmutable;
        UseValueEnvironment()->LookUp(symbol, &isImmutable);
        return isImmutable;
    }

    Type TypeCheck() override;

    void CalculateEscapes() override;
};

struct FieldVar
    : public Var
{
    Symbol symbol;
    std::unique_ptr<Var> var;
    FieldVar(const Symbol& sym, std::unique_ptr<Var>&& invar, const Position& pos)
        : symbol(sym)
        , var(std::move(invar))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
    std::string DumpAST() const override
    {
        return "{FieldVar: " + symbol.UseName() + " : " + var->DumpAST() + "}";
    }

    virtual bool IsImmutable() const override
    {
        return false;
    }

    void CalculateEscapes() override;
};

struct SubscriptVar
    : public Var
{
    std::unique_ptr<Var> var;
    std::unique_ptr<Expression> expression; 
    SubscriptVar(std::unique_ptr<Var>&& inVar, std::unique_ptr<Expression>&& exp, const Position& pos)
        : var(std::move(inVar))
        , expression(std::move(exp)) 
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
   
    std::string DumpAST() const override
    {
        return "{SubscriptVar: " + var->DumpAST() + "[" + expression->DumpAST() +"]}";
    }

    virtual bool IsImmutable() const override
    {
        return false;
    }

    void CalculateEscapes() override;
};

struct VarExpression
    : public Expression
{
    std::unique_ptr<Var> var;
    VarExpression(std::unique_ptr<Var>&& inVar, const Position& pos)
        : var(std::move(inVar)) 
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;
   
    std::string DumpAST() const override
    {
        return "{VarExpression: " + var->DumpAST() + "}";
    }

    void CalculateEscapes() override;
};

struct NilExpression
    : public Expression
{ 
    NilExpression(const Position& pos)
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        return "{NilExpression}";
    }

    void CalculateEscapes() override {}
};

struct IntExpression
    : public Expression
{
    int value;
    IntExpression(int val, const Position& pos)
        : value(val)
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        std::stringstream val;
        val << value;
        return "{IntExpression: " + val.str() + "}";
    }

    void CalculateEscapes() override {}
};

struct StringExpression
    : public Expression
{
    std::string value;
    StringExpression(const std::string& val, const Position& pos)
        : value(val)
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        return "{StringExpression: " + value + "}";
    }

    void CalculateEscapes() override {}
};

struct CallExpression
    : public Expression
{
    Symbol function;
    std::vector<std::unique_ptr<Expression>> args;
    CallExpression(const Symbol& fn, std::vector<std::unique_ptr<Expression>>&& ars, const Position& pos)
        : function(fn)
        , args(std::move(ars))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        std::stringstream ret;
        ret << "{CallExpression: " << function.UseName();
        for (const auto& exp : args)
        {
            ret << exp->DumpAST();
        }
        ret << "}";
        return ret.str();
    }

    void CalculateEscapes() override;
};

struct OpExpression
    : public Expression
{
    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;
    BinOp op;
    OpExpression(std::unique_ptr<Expression>&& ls, std::unique_ptr<Expression>&& rs, BinOp oper, const Position& pos)
        : lhs(std::move(ls))
        , rhs(std::move(rs))
        , op(oper)
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        return "{OpExpression: " + lhs->DumpAST() + DumpOp(op) + rhs->DumpAST() + "}";
    }

    void CalculateEscapes() override;
};

struct FieldExp
{
    Symbol field;
    std::unique_ptr<Expression> expr;
    Position position;

    FieldExp(const Symbol& fld, std::unique_ptr<Expression>&& ex, const Position& pos)
        : field(fld)
        , expr(std::move(ex))
        , position(pos)
    {}

    std::string DumpAST() const
    {
        return "{FieldExp : " + field.UseName() + " : " + expr->DumpAST() + "}";
    }
};

struct RecordExpression
    : public Expression
{
    Symbol type;
    std::vector<FieldExp> fields;
    RecordExpression(const Symbol& ty, std::vector<FieldExp>&& flds, const Position& pos)
        : type(ty)
        , fields(std::move(flds))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        std::stringstream ss;
        ss << "{RecordExpression : " << type.UseName();
        for (const auto& f : fields)
        {
            ss << f.DumpAST();
        }
        ss << "}";
        return ss.str();
    }
    
    void CalculateEscapes() override;
};

struct SeqExpression
    : public Expression
{
    std::vector<std::unique_ptr<Expression>> expressions;
    SeqExpression(std::vector<std::unique_ptr<Expression>>&& expr, const Position& pos)
        : expressions(std::move(expr))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        std::stringstream ss;
        ss << "{SeqExpression : (";
        for (const auto& exp : expressions)
        {
            ss << exp->DumpAST() << ",";
        }
        ss << ")}";
        return ss.str();
    }
    
    void CalculateEscapes() override;
};

struct AssignmentExpression
    : public Expression
{
    std::unique_ptr<Var> var;
    std::unique_ptr<Expression> expression;
    AssignmentExpression(std::unique_ptr<Var>&& v, std::unique_ptr<Expression> expr, const Position& pos)
        : var(std::move(v))
        , expression(std::move(expr))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        return "{AssignmentExpression : " + var->DumpAST() + " := " + expression->DumpAST() + "}";
    }

    void CalculateEscapes() override;
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
            std::unique_ptr<Expression>&& els,
            const Position& pos)
        : test(std::move(t))
        , thenBranch(std::move(thn))
        , elseBranch(std::move(els))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        std::stringstream ss;
        ss << "{IfExpression : if "  << test->DumpAST() << " then " << thenBranch->DumpAST();
        if (elseBranch)
        {
            ss << " else " << elseBranch->DumpAST();
        }
        ss << "}";
        return ss.str();
    }
    void CalculateEscapes() override;
};

struct WhileExpression
    : public Expression
{
    std::unique_ptr<Expression> test;
    std::unique_ptr<Expression> body;

    WhileExpression(std::unique_ptr<Expression>&& t, std::unique_ptr<Expression>&& b, const Position& pos)
        : test(std::move(t))
        , body(std::move(b))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        return "{WhileExpression : test: " + test->DumpAST() + " body : " + body->DumpAST() + "}";
    }

    void CalculateEscapes() override;
};

struct ForExpression
    : public Expression
{
    Symbol var;
    bool escape = false;
    std::unique_ptr<Expression> low;
    std::unique_ptr<Expression> high;
    std::unique_ptr<Expression> body;

    ForExpression(const Symbol& v, std::unique_ptr<Expression>&& l, std::unique_ptr<Expression>&& h, std::unique_ptr<Expression>&& b, const Position& pos)
        : var(v)
        , low(move(l))
        , high(move(h))
        , body(move(b))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        return "{ForExpression : low: " + low->DumpAST() + " high: " + high->DumpAST() + " body : " + body->DumpAST() + "}";
    }
    
    void CalculateEscapes() override;
};

struct BreakExpression
    : public Expression
{
    BreakExpression(const Position& pos)
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        return "{BreakExpression}";
    }
    
    void CalculateEscapes() override {}
};

struct LetExpression
    : public Expression
{
    std::vector<std::unique_ptr<Declaration>> decls;
    std::unique_ptr<Expression> body;

    LetExpression(std::vector<std::unique_ptr<Declaration>>&& decs, std::unique_ptr<Expression>&& bdy, const Position& pos)
        : decls(std::move(decs))
        , body(std::move(bdy))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        std::stringstream ss;
        ss << "{LetExpression : decls: ";
        for (const auto& d : decls)
        {
            ss << d->DumpAST();
        }
        ss << " body: " << body->DumpAST() << "}";
        return ss.str();
    }

    void CalculateEscapes() override;
};

struct ArrayExpression
    : public Expression
{
    Symbol type;
    std::unique_ptr<Expression> size;
    std::unique_ptr<Expression> init;

    ArrayExpression(const Symbol& id, std::unique_ptr<Expression>&& sz, std::unique_ptr<Expression>&& val, const Position& pos)
        : type(id)
        , size(std::move(sz))
        , init(std::move(val))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        return "{ArrayExpression: type: " + type.UseName() + " size: " + size->DumpAST() + " init: " + init->DumpAST() + "}";
    }

    void CalculateEscapes() override;
};

struct FunDec
{
    Symbol name;
    std::vector<Field> fields;
    boost::optional<Symbol> resultTy;
    std::unique_ptr<Expression> body;
    Position position;

    FunDec(const Symbol& nam, std::vector<Field>&& flds, boost::optional<Symbol> ty, std::unique_ptr<Expression>&& bdy, const Position& pos)
        : name(nam)
        , fields(std::move(flds))
        , resultTy(ty)
        , body(std::move(bdy))
        , position(pos)
    {
    }

    std::string DumpAST() const
    {
        std::stringstream ss;
        ss << "{FunDec name: " << name.UseName() << " fields: ";
        for (const auto& f : fields)
        {
            ss << f.DumpAST();
        }
        ss << " result : ";
        if (resultTy)
        {
            ss << resultTy->UseName();
        }
        ss << " body : " << body->DumpAST() << "}";
        return ss.str();
    }
};

struct FunctionDeclaration
    : public Declaration
{
    std::vector<FunDec> decls;

    FunctionDeclaration(std::vector<FunDec>&& decs, const Position& pos)
        : decls(std::move(decs))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        std::stringstream ss;
        ss << "{FunctionDeclaration: ";
        for (const auto& d: decls)
        {
            ss << d.DumpAST();
        }
        ss << "}";
        return ss.str();
    }

    void CalculateEscapes() override;
};

struct VarDeclaration
    : public Declaration
{
   Symbol name;
   bool escape = false;
   boost::optional<Symbol> type;
   std::unique_ptr<Expression> init;

   VarDeclaration(const Symbol& id, boost::optional<Symbol> ty, std::unique_ptr<Expression>&& it, const Position& pos)
    : name(id)
    , type(ty)
    , init(std::move(it))
    {
        SetPosition(pos);
    }

    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        std::stringstream ss;
        ss << "{VarDeclaration: name: " << name.UseName();
        if (type)
        {
            ss << " type: " << type->UseName();
        }
        if (escape)
        {
            ss << " escapes";
        }
        else
        {
            ss << " does not escape";
        }
        ss << " init " << init->DumpAST() << "}";
        return ss.str();
    }

    void CalculateEscapes() override;
};

struct TyDec
{
    Symbol name;
    std::unique_ptr<TypeNode> type;
    Position position;
    TyDec(const Symbol& id, std::unique_ptr<TypeNode>&& ty, const Position& pos)
        : name(id)
        , type(std::move(ty))
        , position(pos)
    {}

    std::string DumpAST() const
    {
        return "{TyDec : name: " + name.UseName() + " type: " + type->DumpAST() + "}";
    }
};

struct TypeDeclaration
    : public Declaration
{
    std::vector<TyDec> types;

    TypeDeclaration(std::vector<TyDec>&& ty, const Position& pos)
        : types(std::move(ty))
    {
        SetPosition(pos);
    }

    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        std::stringstream ss;
        ss << "{TypeDeclaration ";
        for (const auto& t : types)
        {
            ss << t.DumpAST();
        }
        ss << "}";
        return ss.str();
    }
    void CalculateEscapes() override
    {
    }
};

struct NameType
    : public TypeNode
{
    Symbol name;

    NameType(const Symbol& id, const Position& pos)
        : name(id)
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        return "{NameType: " + name.UseName() + "}";
    }
    void CalculateEscapes() override {}
};

struct RecordType
    : public TypeNode
{
    std::vector<Field> fields;

    RecordType(std::vector<Field>&& flds, const Position& pos)
        : fields(std::move(flds))
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        std::stringstream ss;
        ss << "{RecordType ";
        for (const auto& f : fields)
        {
            ss << f.DumpAST();
        }
        ss << "}";
        return ss.str();
    }
    
    void CalculateEscapes() override {}
};

struct ArrayType
    : public TypeNode
{
    Symbol name;

    ArrayType(const Symbol& id, const Position& pos)
        : name(id)
    {
        SetPosition(pos);
    }
    Type TypeCheck() override;

    std::string DumpAST() const override
    {
        return "{ArrayType : name: " +  name.UseName() + "}";
    }
    
    void CalculateEscapes() override {}
};

}
