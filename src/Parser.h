#pragma once
#include "AST.h"
#include "common.h"
#include "TokenStream.h"

#include "CompileTimeErrorHandler.h"

class Parser
{
public:
    Parser(
            TokenStream&& tokenStream,
            const std::shared_ptr<CompileTimeErrorReporter>& errorReporter,
            const std::shared_ptr<WarningReporter>& warningReporter);

    static Parser CreateParserForFile(
            const std::string& file,
            const std::shared_ptr<CompileTimeErrorReporter>& errorReporter,
            const std::shared_ptr<WarningReporter>& warningReporter);

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
    std::unique_ptr<AST::TypeNode> ParseType();
    std::vector<AST::FunDec> ParseFunctionDecls();
    AST::FunDec ParseFunDec();
    std::vector<AST::Field> ParseTyFields();


    TokenStream m_tokenStream;
    std::shared_ptr<CompileTimeErrorReporter> m_errorReporter;
    std::shared_ptr<WarningReporter> m_warningReporter;
};

