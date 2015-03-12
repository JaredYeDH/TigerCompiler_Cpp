#include "Parser.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;
using namespace AST;

BinOp MapPrimTokenToBinOp(std::shared_ptr<CompileTimeErrorReporter>& errorReporter, Token token)
{
	switch (token.GetTokenType())
	{
	case PrimativeToken::Plus:
		return BinOp::Plus;
    case PrimativeToken::Minus:
    	return BinOp::Minus;
    case PrimativeToken::Times:
    	return BinOp::Times;
    case PrimativeToken::Div:
    	return BinOp::Div;
    case PrimativeToken::Equal:
    	return BinOp::Eq;
    case PrimativeToken::NotEqual:
    	return BinOp::Neq;
    case PrimativeToken::LessThan:
    	return BinOp::Lt;
    case PrimativeToken::LEqual:
    	return BinOp::Le;
    case PrimativeToken::GreaterThan:
    	return BinOp::Gt;
    case PrimativeToken::GEqual:
		return BinOp::Ge;
	default:
		break;
	}
    errorReporter->AddError({ErrorCode::Err33, token.UsePosition(), ""});
    // Error correcting
    return BinOp::Eq;
}

Parser::Parser(
        TokenStream&& tokenStream,
        const std::shared_ptr<CompileTimeErrorReporter>& errorReporter,
        const std::shared_ptr<WarningReporter>& warningReporter)
    : m_tokenStream(move(tokenStream))
    , m_errorReporter(errorReporter)
    , m_warningReporter(warningReporter)
{
}

Parser Parser::CreateParserForFile(
        const std::string& file,
        const std::shared_ptr<CompileTimeErrorReporter>& errorReporter,
        const std::shared_ptr<WarningReporter>& warningReporter)
{
    std::unique_ptr<std::istream> stream = make_unique<std::ifstream>(file, std::ifstream::in);
    return Parser(TokenStream(std::move(stream)), errorReporter, warningReporter);
}

unique_ptr<Program> Parser::Parse()
{
    assert(m_errorReporter);
    assert(m_warningReporter);
    return make_unique<Program>(ParseExpression(), m_errorReporter, m_warningReporter);
}

unique_ptr<Expression> Parser::ParseExpression()
{
	return ParseExpOrPR(ParseExpOr());
}

unique_ptr<Expression> Parser::ParseExpOr()
{
	return ParseExpAndPR(ParseExpAnd());
}

unique_ptr<Expression> Parser::ParseExpOrPR(unique_ptr<AST::Expression>&& lhs)
{
    auto peeked = m_tokenStream.PeekNextToken();
	if (peeked.GetTokenType() == PrimativeToken::Or)
	{
		// eat or
		auto eat = m_tokenStream.GetNextToken();
		// e1 | e2 => if e1 then 1 else e2
		unique_ptr<Expression> ifExpr = make_unique<IfExpression>(
			move(lhs),
			make_unique<IntExpression>(1, peeked.UsePosition()),
			ParseExpression(), eat.UsePosition());
        return ifExpr;
	}
	return unique_ptr<Expression>(move(lhs));
}

unique_ptr<Expression> Parser::ParseExpAnd()
{
	// Parse the lhs of the arith and pass it to the relational
	return ParseRelExp(ParseArithExp());
}

unique_ptr<Expression> Parser::ParseExpAndPR(unique_ptr<AST::Expression>&& lhs)
{
    auto peeked = m_tokenStream.PeekNextToken();
	if (peeked.GetTokenType() == PrimativeToken::And)
	{
		// eat and
		auto eat = m_tokenStream.GetNextToken();
		// e1 & e2 => if e1 then e2 else 0
		unique_ptr<Expression> ifExpr = make_unique<IfExpression>(
			move(lhs),
			ParseExpression(),
			make_unique<IntExpression>(0, peeked.UsePosition()), eat.UsePosition());
        return ifExpr;
	}
	return unique_ptr<Expression>(move(lhs));
}

unique_ptr<Expression> Parser::ParseArithExp()
{
	return ParseTermPR(ParseTerm());
}

unique_ptr<Expression> Parser::ParseRelExp(unique_ptr<AST::Expression>&& lhs)
{
	const vector<PrimativeToken> relOps = { PrimativeToken::Equal,
											PrimativeToken::NotEqual,
											PrimativeToken::LessThan,
											PrimativeToken::LEqual,
											PrimativeToken::GreaterThan,
											PrimativeToken::GEqual };

	auto whichOp = find(begin(relOps), end(relOps), m_tokenStream.PeekNextToken().GetTokenType());
	if (whichOp != end(relOps))
	{
		auto opTok = m_tokenStream.GetNextToken();
        Token op = opTok.GetTokenType();
		auto rhs = ParseArithExp();
		return make_unique<OpExpression>(move(lhs), move(rhs), MapPrimTokenToBinOp(m_errorReporter, op), opTok.UsePosition());
	}
	return unique_ptr<Expression>(move(lhs));
}

unique_ptr<Expression> Parser::ParseTerm()
{
	return ParseFactorPR(ParseFactor());
}

unique_ptr<Expression> Parser::ParseTermPR(std::unique_ptr<AST::Expression>&& lhs)
{
	auto primOp = m_tokenStream.PeekNextToken().GetTokenType();
	if (primOp == PrimativeToken::Plus || primOp == PrimativeToken::Minus)
	{
        auto primTok = m_tokenStream.GetNextToken();
		return make_unique<OpExpression>(move(lhs), ParseTermPR(ParseTerm()), MapPrimTokenToBinOp(m_errorReporter, primTok), primTok.UsePosition());
	}
	return unique_ptr<Expression>(move(lhs));
}

unique_ptr<Expression> Parser::ParseFactor()
{
	auto token = m_tokenStream.PeekNextToken().GetTokenType();
	if (token == PrimativeToken::Nil)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		unique_ptr<Expression> expr = make_unique<NilExpression>(eatToken.UsePosition());
        return expr;
	}
	if (token == PrimativeToken::Number)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		int intVal;
		stringstream value(eatToken.UseValue());
		value >> intVal;
		unique_ptr<Expression> expr = make_unique<IntExpression>(intVal, eatToken.UsePosition());
        return expr;
	}
	if (token == PrimativeToken::StringLit)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		unique_ptr<Expression> expr = make_unique<StringExpression>(eatToken.UseValue(), eatToken.UsePosition());
        return expr;
	}
	if (token == PrimativeToken::Break)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		unique_ptr<Expression> expr = make_unique<BreakExpression>(eatToken.UsePosition());
        return expr;
	}
	if (token == PrimativeToken::LParen)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		auto exprList = ParseExpressionList();
		if (m_tokenStream.PeekNextToken().GetTokenType() != PrimativeToken::RParen)
		{
            m_errorReporter->AddError({ErrorCode::Err34, m_tokenStream.PeekNextToken().UsePosition(), ""});
		}
        else
        {
		    m_tokenStream.GetNextToken();
        }
        return exprList;
	}
	if (token == PrimativeToken::Minus)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		unique_ptr<Expression> expr =  make_unique<OpExpression>(
			make_unique<IntExpression>(0, eatToken.UsePosition()),
			ParseExpression(),
			BinOp::Minus, eatToken.UsePosition());
        return expr;
	}
	if (token == PrimativeToken::If)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		auto ifBranch = ParseExpression();
		if (m_tokenStream.PeekNextToken().GetTokenType() != PrimativeToken::Then)
		{
            m_errorReporter->AddError({ErrorCode::Err35, m_tokenStream.PeekNextToken().UsePosition(), ""});
		}
        else
        {
		    m_tokenStream.GetNextToken();
        }
		auto thenBranch = ParseExpression();
		
		// nullable else
		unique_ptr<Expression> elseBranch;
		if (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Else)
		{
			m_tokenStream.GetNextToken();
			elseBranch = ParseExpression();
		}
		unique_ptr<Expression> expr = make_unique<IfExpression>(move(ifBranch), move(thenBranch), move(elseBranch), eatToken.UsePosition());
        return expr;
	}
	if (token == PrimativeToken::While)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		auto cond = ParseExpression();
		if (m_tokenStream.PeekNextToken().GetTokenType() != PrimativeToken::Do)
		{
			m_errorReporter->AddError({ErrorCode::Err36, m_tokenStream.PeekNextToken().UsePosition(), ""});
		}
        else
        {
		    m_tokenStream.GetNextToken();
        }
		auto body = ParseExpression();
		unique_ptr<Expression> expr = make_unique<WhileExpression>(move(cond), move(body), eatToken.UsePosition());
        return expr;
	}
	if (token == PrimativeToken::For)
	{
		auto eatToken = m_tokenStream.GetNextToken();
        auto forPos = eatToken.UsePosition();
		auto var = m_tokenStream.PeekNextToken();
		if (var.GetTokenType() != PrimativeToken::Identifier)
		{
			m_errorReporter->AddError({ErrorCode::Err37, var.UsePosition(), ""});
		}
        else
        {
            var = m_tokenStream.GetNextToken();
        }
		eatToken = m_tokenStream.PeekNextToken();
		if (eatToken.GetTokenType() != PrimativeToken::Assign)
		{
            m_errorReporter->AddError({ErrorCode::Err38, eatToken.UsePosition(), ""});
		}
        else
        {
            eatToken = m_tokenStream.GetNextToken();
        }
		auto init = ParseExpression();
		eatToken = m_tokenStream.PeekNextToken();
		if (eatToken.GetTokenType() != PrimativeToken::To)
		{
            m_errorReporter->AddError({ErrorCode::Err39, eatToken.UsePosition(), ""});
		}
        else
        {
            eatToken = m_tokenStream.GetNextToken();
        }
		auto range = ParseExpression();
		eatToken = m_tokenStream.PeekNextToken();
		if (eatToken.GetTokenType() != PrimativeToken::Do)
		{
            m_errorReporter->AddError({ErrorCode::Err40, eatToken.UsePosition(), ""});
		}
        else
        {
            eatToken = m_tokenStream.GetNextToken();
        }
		auto body = ParseExpression();
		unique_ptr<Expression> expr = make_unique<ForExpression>(SymbolFactory::GenerateSymbol(var.UseValue()), move(init), move(range), move(body), forPos);
        return expr;
	}
	if (token == PrimativeToken::Let)
	{
		auto eatToken = m_tokenStream.GetNextToken();
        auto letPos = eatToken.UsePosition();
		auto decs = ParseDeclList();
        auto expectIn = m_tokenStream.PeekNextToken();
		if (expectIn.GetTokenType() != PrimativeToken::In)
		{
            m_errorReporter->AddError({ErrorCode::Err41, expectIn.UsePosition(), ""});
		}
        else
        {
            expectIn = m_tokenStream.GetNextToken();
        }
		unique_ptr<Expression> expr;
		// explicitly handle empty expressions
		if (m_tokenStream.PeekNextToken().GetTokenType() != PrimativeToken::End)
		{
			expr = ParseExpressionList();
		}
		else
		{
			expr = make_unique<SeqExpression>(vector<unique_ptr<Expression>>(), m_tokenStream.PeekNextToken().UsePosition());
		}
		eatToken = m_tokenStream.PeekNextToken();
		if (eatToken.GetTokenType() != PrimativeToken::End)
        {
            m_errorReporter->AddError({ErrorCode::Err42, eatToken.UsePosition(), ""});
		}
        else
        {
            eatToken = m_tokenStream.GetNextToken();
        }
		unique_ptr<Expression> letExp = make_unique<LetExpression>(move(decs), move(expr), letPos);
        return letExp;
	}
	return ParseLValue();
}

unique_ptr<Expression> Parser::ParseFactorPR(std::unique_ptr<AST::Expression>&& lhs)
{
	auto primOp = m_tokenStream.PeekNextToken().GetTokenType();
	if (primOp == PrimativeToken::Times || primOp == PrimativeToken::Div)
	{
        auto primTok = m_tokenStream.GetNextToken();
		return make_unique<OpExpression>(move(lhs), ParseFactorPR(ParseFactor()), MapPrimTokenToBinOp(m_errorReporter, primTok), primTok.UsePosition());
	}
	return unique_ptr<Expression>(move(lhs));
}

unique_ptr<AST::Expression> Parser::ParseExpressionList()
{
	if (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::RParen)
	{
		return make_unique<SeqExpression>(vector<unique_ptr<Expression>>(), m_tokenStream.PeekNextToken().UsePosition());
	}

	auto expression = ParseExpression();
	if (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Semi)
	{
        Position pos = m_tokenStream.PeekNextToken().UsePosition();
		vector<unique_ptr<Expression>> exprs;
		exprs.push_back(move(expression));
		do
		{
			auto eat = m_tokenStream.GetNextToken();
			exprs.push_back(ParseExpression());
		} while (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Semi);
		return make_unique<SeqExpression>(move(exprs), pos);
	}
	return expression;
}

unique_ptr<AST::Expression> Parser::ParseLValue()
{
	auto token = m_tokenStream.PeekNextToken();
	if (token.GetTokenType() != PrimativeToken::Identifier)
    {
        m_errorReporter->AddError({ErrorCode::Err43, token.UsePosition(), ""});
	}
    else
    {
        token = m_tokenStream.GetNextToken();
    }
	return ParseFunRecArr(token);
}

unique_ptr<AST::Expression> Parser::ParseFunRecArr(const Token& id)
{
	auto token = m_tokenStream.PeekNextToken().GetTokenType();
	// fun call
	if (token == PrimativeToken::LParen)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		auto argList = ParseArgList();
        auto close = m_tokenStream.PeekNextToken();
		if (close.GetTokenType() != PrimativeToken::RParen)
        {
            m_errorReporter->AddError({ErrorCode::Err44, close.UsePosition(), ""});
		}
        else
        {
            close = m_tokenStream.GetNextToken();
        }
		return make_unique<CallExpression>(SymbolFactory::GenerateSymbol(id.UseValue()), move(argList), eatToken.UsePosition());
	}
	// record
	if (token == PrimativeToken::LBrace)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		auto fieldList = ParseFieldList();
        auto brace = m_tokenStream.PeekNextToken();
		if (brace.GetTokenType() != PrimativeToken::RBrace)
        {
            m_errorReporter->AddError({ErrorCode::Err45, brace.UsePosition(), ""});
		}
        else
        {
            brace = m_tokenStream.GetNextToken();
        }
		return make_unique<RecordExpression>(SymbolFactory::GenerateSymbol(id.UseValue()), move(fieldList), id.UsePosition());
	}
	// array
	if (token == PrimativeToken::LBracket)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		auto size = ParseExpression();
        auto bracket = m_tokenStream.PeekNextToken();
		if (bracket.GetTokenType() != PrimativeToken::RBracket)
		{
            m_errorReporter->AddError({ErrorCode::Err46, bracket.UsePosition(), ""});
		}
        else
        {
            bracket = m_tokenStream.GetNextToken();
        }

		if (m_tokenStream.PeekNextToken().GetTokenType() != PrimativeToken::Of)
		{
			auto simple = make_unique<SimpleVar>(SymbolFactory::GenerateSymbol(id.UseValue()), id.UsePosition());
			return ParseFunRecArrPR(make_unique<SubscriptVar>(move(simple), move(size), eatToken.UsePosition()));
		}
		else
		{
			m_tokenStream.GetNextToken();
		}
		auto init = ParseExpression();
		return make_unique<ArrayExpression>(SymbolFactory::GenerateSymbol(id.UseValue()), move(size), move(init), eatToken.UsePosition());
	}
	else
	{
		return ParseFunRecArrPR(make_unique<SimpleVar>(SymbolFactory::GenerateSymbol(id.UseValue()), id.UsePosition()));
	}
}

unique_ptr<AST::Expression> Parser::ParseFunRecArrPR(unique_ptr<Var>&& inVar)
{
	std::unique_ptr<Var> var = move(inVar);

	auto nextToken = m_tokenStream.PeekNextToken().GetTokenType();
	while (nextToken == PrimativeToken::Period || nextToken == PrimativeToken::LBracket)
	{
		nextToken = m_tokenStream.GetNextToken().GetTokenType();
		if (nextToken == PrimativeToken::Period)
		{
			auto field = m_tokenStream.PeekNextToken();
			if (field.GetTokenType() != PrimativeToken::Identifier)
			{
                m_errorReporter->AddError({ErrorCode::Err47, field.UsePosition(), ""});
			}
            else
            {
                field = m_tokenStream.GetNextToken();
            }
			var = make_unique<FieldVar>(SymbolFactory::GenerateSymbol(field.UseValue()), std::move(var), field.UsePosition());
		}
		else
		{
            auto pos = m_tokenStream.PeekNextToken().UsePosition();
			auto expr = ParseExpression();
			var = make_unique<SubscriptVar>(move(var), move(expr), pos);
            auto closeBracket = m_tokenStream.PeekNextToken();
			if (closeBracket.GetTokenType() != PrimativeToken::RBracket)
			{
                m_errorReporter->AddError({ErrorCode::Err48, closeBracket.UsePosition(), ""});
			}
            else
            {
                closeBracket = m_tokenStream.GetNextToken();
            }
		}
		nextToken = m_tokenStream.PeekNextToken().GetTokenType();
	}

	if (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Assign)
	{
		auto eat = m_tokenStream.GetNextToken();
		auto val = ParseExpression();
		return make_unique<AssignmentExpression>(move(var), move(val), eat.UsePosition());
	}

	return make_unique<VarExpression>(move(var), m_tokenStream.PeekNextToken().UsePosition());
}

vector<unique_ptr<AST::Expression>> Parser::ParseArgList()
{
	vector<unique_ptr<AST::Expression>> args;
	// Don't eat this token
	if (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::RParen)
	{
		return args;
	}
	auto first = ParseExpression();
	args.push_back(move(first));
	if (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Comma)
	{
		do
		{
			auto eatToken = m_tokenStream.GetNextToken();
			args.push_back(ParseExpression());
		} while (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Comma);
	}
	return args;
}

vector<FieldExp> Parser::ParseFieldList()
{
	vector<FieldExp> fields;
	while (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Identifier)
	{
		auto id = m_tokenStream.GetNextToken();
        auto equals = m_tokenStream.PeekNextToken();
		if (equals.GetTokenType() != PrimativeToken::Equal)
		{
            m_errorReporter->AddError({ErrorCode::Err49, equals.UsePosition(), ""});
		}
        else
        {
            equals = m_tokenStream.GetNextToken();
        }

		auto val = ParseExpression();
		fields.push_back(FieldExp(SymbolFactory::GenerateSymbol(id.UseValue()), move(val), id.UsePosition()));

		if (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Comma)
		{
			m_tokenStream.GetNextToken();
		}
		else
		{
			break;
		}
	}
	return fields;
}

unique_ptr<Declaration> Parser::ParseDecl()
{
	auto token = m_tokenStream.PeekNextToken().GetTokenType();
	if (token == PrimativeToken::Type)
	{
		vector<TyDec> types;
        auto pos = m_tokenStream.PeekNextToken().UsePosition();
		while (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Type)
		{
			auto eatToken = m_tokenStream.GetNextToken();
			auto id = m_tokenStream.PeekNextToken();
			if (id.GetTokenType() != PrimativeToken::Identifier)
			{	
                m_errorReporter->AddError({ErrorCode::Err50, id.UsePosition(), ""});
			}
            else
            {
                id = m_tokenStream.GetNextToken();
            }

            auto equals = m_tokenStream.PeekNextToken();
			if (equals.GetTokenType() != PrimativeToken::Equal)
			{
                m_errorReporter->AddError({ErrorCode::Err51, equals.UsePosition(), ""});
			}
            else
            {
                equals = m_tokenStream.GetNextToken();
            }

			types.push_back(TyDec(SymbolFactory::GenerateSymbol(id.UseValue()), ParseType(), id.UsePosition()));
		}
		return make_unique<TypeDeclaration>(move(types), pos);
	}
	if (token == PrimativeToken::Var)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		auto id = m_tokenStream.PeekNextToken();
		if (id.GetTokenType() != PrimativeToken::Identifier)
		{
            m_errorReporter->AddError({ErrorCode::Err52, id.UsePosition(), ""});
		}
        else
        {
            id = m_tokenStream.GetNextToken();
        }

        boost::optional<Symbol> ty;
		// optional type annotation
		if (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Colon)
		{
			eatToken = m_tokenStream.GetNextToken();
			ty = SymbolFactory::GenerateSymbol(m_tokenStream.GetNextToken().UseValue());
		}

        auto assign = m_tokenStream.PeekNextToken();
		if (assign.GetTokenType() != PrimativeToken::Assign)
		{
            m_errorReporter->AddError({ErrorCode::Err53, assign.UsePosition(), ""});
		}
        else
        {
            assign = m_tokenStream.GetNextToken();
        }

        return make_unique<VarDeclaration>(SymbolFactory::GenerateSymbol(id.UseValue()), ty, ParseExpression(), id.UsePosition());
	}
	if (token == PrimativeToken::Function)
	{
		// don't eat the function token.
		return make_unique<FunctionDeclaration>(ParseFunctionDecls(), m_tokenStream.PeekNextToken().UsePosition());
	}

	return nullptr;
}

vector<unique_ptr<Declaration>> Parser::ParseDeclList()
{
	vector<unique_ptr<Declaration>> decls;
	unique_ptr<Declaration> decl;
	while (decl = ParseDecl())
	{
		decls.push_back(std::move(decl));
	}
	return decls;
}

vector<FunDec> Parser::ParseFunctionDecls()
{
	vector<FunDec> decls;
	// don't eat the function token
	while (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Function)
	{
		decls.push_back(ParseFunDec());
	}
	return decls;
}

FunDec Parser::ParseFunDec()
{
    auto fun = m_tokenStream.PeekNextToken();
	if (fun.GetTokenType() != PrimativeToken::Function)
	{
        m_errorReporter->AddError({ErrorCode::Err54, fun.UsePosition(), ""});
	}
    else
    {
        fun = m_tokenStream.GetNextToken();
    }

	auto id = m_tokenStream.PeekNextToken();
	if (id.GetTokenType() != PrimativeToken::Identifier)
	{
        m_errorReporter->AddError({ErrorCode::Err55, id.UsePosition(), ""});
	}
    else
    {
        id = m_tokenStream.GetNextToken();
    }

    auto lparen = m_tokenStream.PeekNextToken();
	if (lparen.GetTokenType() != PrimativeToken::LParen)
	{
        m_errorReporter->AddError({ErrorCode::Err56, lparen.UsePosition(), ""});
	}
    else
    {
        lparen = m_tokenStream.GetNextToken();
    }

	vector<Field> fieldList = ParseTyFields();
    auto rparen = m_tokenStream.PeekNextToken();
	if (rparen.GetTokenType() != PrimativeToken::RParen)
	{
        m_errorReporter->AddError({ErrorCode::Err57, rparen.UsePosition(), ""});
	}
    else
    {
        rparen = m_tokenStream.GetNextToken();
    }

    boost::optional<Symbol> ty;
	if (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Colon)
	{
		m_tokenStream.GetNextToken();
		auto tyTok = m_tokenStream.PeekNextToken();
		if (tyTok.GetTokenType() != PrimativeToken::Identifier)
		{
            m_errorReporter->AddError({ErrorCode::Err58, tyTok.UsePosition(), ""});
		}
        else
        {
            tyTok = m_tokenStream.GetNextToken();
        }

		ty = SymbolFactory::GenerateSymbol(tyTok.UseValue());
	}
    auto equal = m_tokenStream.PeekNextToken();
	if (equal.GetTokenType() != PrimativeToken::Equal)
	{
        m_errorReporter->AddError({ErrorCode::Err59, equal.UsePosition(), ""});
	}
    else
    {
        equal = m_tokenStream.GetNextToken();
    }

	auto body = ParseExpression();

	return FunDec(SymbolFactory::GenerateSymbol(id.UseValue()), move(fieldList), ty, move(body), id.UsePosition());
}

unique_ptr<TypeNode> Parser::ParseType()
{
	auto token = m_tokenStream.GetNextToken();
	if (token.GetTokenType() == PrimativeToken::Identifier)
	{
		return make_unique<NameType>(SymbolFactory::GenerateSymbol(token.UseValue()), token.UsePosition());
	}
	if (token.GetTokenType() == PrimativeToken::LBrace)
	{
		auto recType = ParseTyFields();
        auto brace = m_tokenStream.PeekNextToken();
		if (brace.GetTokenType() != PrimativeToken::RBrace)
		{
            m_errorReporter->AddError({ErrorCode::Err60, brace.UsePosition(), ""});
		}
        else
        {
            brace = m_tokenStream.GetNextToken();
        }

		return make_unique<RecordType>(std::move(recType), token.UsePosition());
	}
	if (token.GetTokenType() == PrimativeToken::Array)
	{
        auto of = m_tokenStream.PeekNextToken();
		if (of.GetTokenType() != PrimativeToken::Of)
		{
            m_errorReporter->AddError({ErrorCode::Err61, of.UsePosition(), ""});
		}
        else
        {
            of = m_tokenStream.GetNextToken();
        }

		auto ty = m_tokenStream.PeekNextToken();
		if (ty.GetTokenType() != PrimativeToken::Identifier)
		{
            m_errorReporter->AddError({ErrorCode::Err62, ty.UsePosition(), ""});
		}
        else
        {
            ty = m_tokenStream.GetNextToken();
        }

		return make_unique<ArrayType>(SymbolFactory::GenerateSymbol(ty.UseValue()), ty.UsePosition());
	}

    m_errorReporter->AddError({ErrorCode::Err63, token.UsePosition(), ""});
    // error correction
    return make_unique<ArrayType>(SymbolFactory::GenerateSymbol("int"), token.UsePosition());
}

vector<Field> Parser::ParseTyFields()
{
	vector<Field> fields;
	
	while (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Identifier)
	{
		auto id = m_tokenStream.GetNextToken();

        auto colon = m_tokenStream.PeekNextToken();
		if (colon.GetTokenType() != PrimativeToken::Colon)
		{
            m_errorReporter->AddError({ErrorCode::Err64, colon.UsePosition(), ""});
		}
        else
        {
            colon = m_tokenStream.GetNextToken();
        }

		auto val = m_tokenStream.PeekNextToken();
		if (val.GetTokenType() != PrimativeToken::Identifier)
		{
            m_errorReporter->AddError({ErrorCode::Err65, val.UsePosition(), ""});
		}
        else
        {
            val = m_tokenStream.GetNextToken();
        }

		fields.push_back(Field(SymbolFactory::GenerateSymbol(id.UseValue()), SymbolFactory::GenerateSymbol(val.UseValue()), id.UsePosition()));

		if (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Comma)
		{
			m_tokenStream.GetNextToken();
		}
		else
		{
			break;
		}
	}

	return fields;
}
