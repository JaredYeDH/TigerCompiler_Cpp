#include "Parser.h"
#include <algorithm>
#include <sstream>
#include <iostream>

using namespace std;
using namespace AST;

BinOp MapPrimTokenToBinOp(PrimativeToken token)
{
	switch (token)
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
	throw ParseException("Expected Binary operator");
}

Parser::Parser(TokenStream&& stream)
    : m_tokenStream(move(stream))
{
}

unique_ptr<Program> Parser::Parse()
{
    return make_unique<Program>(ParseExpression());
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
	if (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Or)
	{
		// eat or
		m_tokenStream.GetNextToken();
		// e1 | e2 => if e1 then 1 else e2
		return make_unique<IfExpression>(
			move(lhs),
			make_unique<IntExpression>(1),
			ParseExpression());
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
	if (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::And)
	{
		// eat and
		m_tokenStream.GetNextToken();
		// e1 & e2 => if e1 then e2 else 0
		return make_unique<IfExpression>(
			move(lhs),
			ParseExpression(),
			make_unique<IntExpression>(0));
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
		PrimativeToken primOp = m_tokenStream.GetNextToken().GetTokenType();
		auto rhs = ParseArithExp();
		return make_unique<OpExpression>(move(lhs), move(rhs), MapPrimTokenToBinOp(primOp));
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
		PrimativeToken primOp = m_tokenStream.GetNextToken().GetTokenType();
		return make_unique<OpExpression>(move(lhs), ParseTermPR(ParseTerm()), MapPrimTokenToBinOp(primOp));
	}
	return unique_ptr<Expression>(move(lhs));
}

unique_ptr<Expression> Parser::ParseFactor()
{
	auto token = m_tokenStream.PeekNextToken().GetTokenType();
	if (token == PrimativeToken::Nil)
	{
		m_tokenStream.GetNextToken();
		return make_unique<NilExpression>();
	}
	if (token == PrimativeToken::Number)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		int intVal;
		stringstream value(eatToken.UseValue());
		value >> intVal;
		return make_unique<IntExpression>(intVal);
	}
	if (token == PrimativeToken::StringLit)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		return make_unique<StringExpression>(eatToken.UseValue());
	}
	if (token == PrimativeToken::Break)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		return make_unique<BreakExpression>();
	}
	if (token == PrimativeToken::LParen)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		auto exprList = ParseExpressionList();
		if (m_tokenStream.PeekNextToken().GetTokenType() != PrimativeToken::RParen)
		{
			throw ParseException("Unclosed paren");
		}
		eatToken = m_tokenStream.GetNextToken();
		return exprList;
	}
	if (token == PrimativeToken::Minus)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		return make_unique<OpExpression>(
			make_unique<IntExpression>(0),
			ParseExpression(),
			BinOp::Minus);
	}
	if (token == PrimativeToken::If)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		auto ifBranch = ParseExpression();
		if (m_tokenStream.PeekNextToken().GetTokenType() != PrimativeToken::Then)
		{
			throw ParseException("If expression with no then");
		}
		eatToken = m_tokenStream.GetNextToken();
		auto thenBranch = ParseExpression();
		
		// nullable else
		unique_ptr<Expression> elseBranch;
		if (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Else)
		{
			eatToken = m_tokenStream.GetNextToken();
			elseBranch = ParseExpression();
		}
		return make_unique<IfExpression>(move(ifBranch), move(thenBranch), move(elseBranch));
	}
	if (token == PrimativeToken::While)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		auto cond = ParseExpression();
		if (m_tokenStream.PeekNextToken().GetTokenType() != PrimativeToken::Do)
		{
			throw ParseException("Expected do after condition of while");
		}
		eatToken = m_tokenStream.GetNextToken();
		auto body = ParseExpression();
		return make_unique<WhileExpression>(move(cond), move(body));
	}
	if (token == PrimativeToken::For)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		auto var = m_tokenStream.GetNextToken();
		if (var.GetTokenType() != PrimativeToken::Identifier)
		{
			throw ParseException("Expected a variable declaration at start of for");
		}
		eatToken = m_tokenStream.GetNextToken();
		if (eatToken.GetTokenType() != PrimativeToken::Assign)
		{
			throw ParseException("Variable in for loop not assigned initial value");
		}
		auto init = ParseExpression();
		eatToken = m_tokenStream.GetNextToken();
		if (eatToken.GetTokenType() != PrimativeToken::To)
		{
			throw ParseException("Expected to in for loop");
		}
		auto range = ParseExpression();
		eatToken = m_tokenStream.GetNextToken();
		if (eatToken.GetTokenType() != PrimativeToken::Do)
		{
			throw ParseException("Expected do after header of for loop");
		}
		auto body = ParseExpression();
		return make_unique<ForExpression>(var.UseValue(), move(init), move(range), move(body));
	}
	if (token == PrimativeToken::Let)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		auto decs = ParseDeclList();
		if (m_tokenStream.GetNextToken().GetTokenType() != PrimativeToken::In)
		{
			throw ParseException("Expected in following declaration section of let expression");
		}
		unique_ptr<Expression> expr;
		// explicitly handle empty expressions
		if (m_tokenStream.PeekNextToken().GetTokenType() != PrimativeToken::End)
		{
			expr = ParseExpressionList();
		}
		else
		{
			expr = make_unique<SeqExpression>(vector<unique_ptr<Expression>>());
		}
		eatToken = m_tokenStream.GetNextToken();
		if (eatToken.GetTokenType() != PrimativeToken::End)
		{
			throw ParseException("Expected end following let expression");
		}
		return make_unique<LetExpression>(move(decs), move(expr));
	}
	return ParseLValue();
}

unique_ptr<Expression> Parser::ParseFactorPR(std::unique_ptr<AST::Expression>&& lhs)
{
	auto primOp = m_tokenStream.PeekNextToken().GetTokenType();
	if (primOp == PrimativeToken::Times || primOp == PrimativeToken::Div)
	{
		PrimativeToken primOp = m_tokenStream.GetNextToken().GetTokenType();
		return make_unique<OpExpression>(move(lhs), ParseFactorPR(ParseFactor()), MapPrimTokenToBinOp(primOp));
	}
	return unique_ptr<Expression>(move(lhs));
}

unique_ptr<AST::Expression> Parser::ParseExpressionList()
{
	if (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::RParen)
	{
		return make_unique<SeqExpression>(vector<unique_ptr<Expression>>());
	}

	auto expression = ParseExpression();
	if (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Semi)
	{
		vector<unique_ptr<Expression>> exprs;
		exprs.push_back(move(expression));
		do
		{
			auto eat = m_tokenStream.GetNextToken();
			exprs.push_back(ParseExpression());
		} while (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Semi);
		return make_unique<SeqExpression>(move(exprs));
	}
	return expression;
}

unique_ptr<AST::Expression> Parser::ParseLValue()
{
	auto token = m_tokenStream.GetNextToken();
	if (token.GetTokenType() != PrimativeToken::Identifier)
	{
		throw ParseException("Malformed LValue");
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
		if (m_tokenStream.GetNextToken().GetTokenType() != PrimativeToken::RParen)
		{
			throw ParseException("Unclosed paren following call expression");
		}
		return make_unique<CallExpression>(id.UseValue(), move(argList));
	}
	// record
	if (token == PrimativeToken::LBrace)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		auto fieldList = ParseFieldList();
		if (m_tokenStream.GetNextToken().GetTokenType() != PrimativeToken::RBrace)
		{
			throw ParseException("Unclosed brace following record");
		}
		return make_unique<RecordExpression>(id.UseValue(), move(fieldList));
	}
	// array
	if (token == PrimativeToken::LBracket)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		auto size = ParseExpression();
		if (m_tokenStream.GetNextToken().GetTokenType() != PrimativeToken::RBracket)
		{
			throw ParseException("Unclosed bracket following array");
		}
		if (m_tokenStream.PeekNextToken().GetTokenType() != PrimativeToken::Of)
		{
			auto simple = make_unique<SimpleVar>(id.UseValue());
			return ParseFunRecArrPR(make_unique<SubscriptVar>(move(simple), move(size)));
		}
		else
		{
			m_tokenStream.GetNextToken();
		}
		auto init = ParseExpression();
		return make_unique<ArrayExpression>(id.UseValue(), move(size), move(init));
	}
	else
	{
		return ParseFunRecArrPR(make_unique<SimpleVar>(id.UseValue()));
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
			auto field = m_tokenStream.GetNextToken();
			if (field.GetTokenType() != PrimativeToken::Identifier)
			{
				throw ParseException("Expected field following .");
			}
			var = make_unique<FieldVar>(field.UseValue(), std::move(var));
		}
		else
		{
			auto expr = ParseExpression();
			var = make_unique<SubscriptVar>(move(var), move(expr));
			if (m_tokenStream.GetNextToken().GetTokenType() != PrimativeToken::RBracket)
			{
				throw ParseException("Exprected ] following subscript");
			}
		}
		nextToken = m_tokenStream.PeekNextToken().GetTokenType();
	}

	if (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Assign)
	{
		m_tokenStream.GetNextToken();
		auto val = ParseExpression();
		return make_unique<AssignmentExpression>(move(var), move(val));
	}

	return make_unique<VarExpression>(move(var));
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
		if (m_tokenStream.GetNextToken().GetTokenType() != PrimativeToken::Equal)
		{
			throw ParseException("Expected = after id in type fields");
		}
		auto val = ParseExpression();
		fields.push_back(FieldExp(id.UseValue(), move(val)));

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
		while (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Type)
		{
			auto eatToken = m_tokenStream.GetNextToken();
			auto id = m_tokenStream.GetNextToken();
			if (id.GetTokenType() != PrimativeToken::Identifier)
			{	
				throw ParseException("Expected Identifier naming type declaration");
			}
			if (m_tokenStream.GetNextToken().GetTokenType() != PrimativeToken::Equal)
			{
				throw ParseException("Expected = after name in TypeDeclaration");
			}

			types.push_back(TyDec(id.UseValue(), ParseType()));
		}
		return make_unique<TypeDeclaration>(move(types));
	}
	if (token == PrimativeToken::Var)
	{
		auto eatToken = m_tokenStream.GetNextToken();
		auto id = m_tokenStream.GetNextToken();
		if (id.GetTokenType() != PrimativeToken::Identifier)
		{
			throw ParseException("Expected Identifier following keyword var");
		}
		std::string ty;
		// optional type annotation
		if (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Colon)
		{
			eatToken = m_tokenStream.GetNextToken();
			ty = m_tokenStream.GetNextToken().UseValue();
		}

		if (m_tokenStream.GetNextToken().GetTokenType() != PrimativeToken::Assign)
		{
			throw ParseException("No assignment operator following Var declaration");
		}

		return make_unique<VarDeclaration>(id.UseValue(), ty, ParseExpression());
	}
	if (token == PrimativeToken::Function)
	{
		// don't eat the function token.
		return make_unique<FunctionDeclaration>(ParseFunctionDecls());
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
	if (m_tokenStream.GetNextToken().GetTokenType() != PrimativeToken::Function)
	{
		throw ParseException("Error parsing function. Expected keyword function");
	}
	auto id = m_tokenStream.GetNextToken();
	if (id.GetTokenType() != PrimativeToken::Identifier)
	{
		throw ParseException("Expected id to follow function keyword for declaration");
	}
	if (m_tokenStream.GetNextToken().GetTokenType() != PrimativeToken::LParen)
	{
		throw ParseException("Expected ( following name for function declaration");
	}
	vector<Field> fieldList = ParseTyFields();
	if (m_tokenStream.GetNextToken().GetTokenType() != PrimativeToken::RParen)
	{
		throw ParseException("Expected ) following parameter list for function declaration");
	}
	Symbol ty;
	if (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Colon)
	{
		m_tokenStream.GetNextToken();
		auto tyTok = m_tokenStream.GetNextToken();
		if (tyTok.GetTokenType() != PrimativeToken::Identifier)
		{
			throw ParseException("Expected type literal for type annotation");
		}
		ty = tyTok.UseValue();
	}
	if (m_tokenStream.GetNextToken().GetTokenType() != PrimativeToken::Equal)
	{
		throw ParseException("Expected = to preceed body of function");
	}
	auto body = ParseExpression();

	return FunDec(id.UseValue(), move(fieldList), ty, move(body));
}

unique_ptr<Type> Parser::ParseType()
{
	auto token = m_tokenStream.GetNextToken();
	if (token.GetTokenType() == PrimativeToken::Identifier)
	{
		return make_unique<NameType>(token.UseValue());
	}
	if (token.GetTokenType() == PrimativeToken::LBrace)
	{
		auto recType = ParseTyFields();
		if (m_tokenStream.GetNextToken().GetTokenType() != PrimativeToken::RBrace)
		{
			throw ParseException("Unclosed brace following Record declaration");
		}
		return make_unique<RecordType>(std::move(recType));
	}
	if (token.GetTokenType() == PrimativeToken::Array)
	{
		if (m_tokenStream.GetNextToken().GetTokenType() != PrimativeToken::Of)
		{
			throw ParseException("Expected keyword of following array");
		}
		auto ty = m_tokenStream.GetNextToken();
		if (ty.GetTokenType() != PrimativeToken::Identifier)
		{
			throw ParseException("Expected type literal for array type");
		}
		return make_unique<ArrayType>(ty.UseValue());
	}
	throw ParseException("Invalid type");
}

vector<Field> Parser::ParseTyFields()
{
	vector<Field> fields;
	
	while (m_tokenStream.PeekNextToken().GetTokenType() == PrimativeToken::Identifier)
	{
		auto id = m_tokenStream.GetNextToken();
		if (m_tokenStream.GetNextToken().GetTokenType() != PrimativeToken::Colon)
		{
			throw ParseException("Expected colon after id in type fields");
		}
		auto val = m_tokenStream.GetNextToken();
		if (val.GetTokenType() != PrimativeToken::Identifier)
		{
			throw ParseException("Expected type literal for type annotation following id :");
		}
		fields.push_back(Field(id.UseValue(), val.UseValue()));

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
