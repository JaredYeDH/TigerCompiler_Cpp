#pragma once
#include "AST.h"
#include "common.h"
#include "TokenStream.h"

class ParseException
    : public std::exception
{
public:
    ParseException(const char* message)
        : m_message(message)
    {}

    virtual const char* what() const throw() override
    {
        return m_message.c_str();
    }

private:
    std::string m_message;
};

class Parser
{
public:
    Parser(TokenStream&& tokenStream);

    std::unique_ptr<AST::Program> Parse();
private:
    std::unique_ptr<AST::Expression> ParseExpression();
    std::unique_ptr<AST::Expression> ParseExpOr();
    std::unique_ptr<AST::Expression> ParseExpAnd();
    std::unique_ptr<AST::Expression> ParseExpOrPR(std::unique_ptr<AST::Expression>&& lhs);
    std::unique_ptr<AST::Expression> ParseExpAndPR(std::unique_ptr<AST::Expression>&& lhs);
    std::unique_ptr<AST::Expression> ParseArithExp();
    std::unique_ptr<AST::Expression> ParseRelExp(std::unique_ptr<AST::Expression>&& lhs);
    std::unique_ptr<AST::Expression> ParseTerm();
    std::unique_ptr<AST::Expression> ParseTermPR(std::unique_ptr<AST::Expression>&& lhs);
    std::unique_ptr<AST::Expression> ParseFactor();
    std::unique_ptr<AST::Expression> ParseFactorPR(std::unique_ptr<AST::Expression>&& lhs);
    std::unique_ptr<AST::Expression> ParseLValue();
    std::unique_ptr<AST::Expression> ParseExpressionList();
    std::unique_ptr<AST::Expression> ParseFunRecArr(const Token& id);
    std::unique_ptr<AST::Expression> ParseFunRecArrPR(std::unique_ptr<AST::Var>&& var);
    std::vector<std::unique_ptr<AST::Expression>> ParseArgList();
    std::vector<AST::FieldExp> ParseFieldList();
    std::unique_ptr<AST::Declaration> ParseDecl();
    std::vector<std::unique_ptr<AST::Declaration>> ParseDeclList();
    std::unique_ptr<AST::Type> ParseType();
    std::vector<AST::FunDec> ParseFunctionDecls();
    AST::FunDec ParseFunDec();
    std::vector<AST::Field> ParseTyFields();


    TokenStream m_tokenStream;
};

