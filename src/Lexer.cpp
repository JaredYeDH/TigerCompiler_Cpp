#include "Lexer.h"
#include <sstream>

Lexer::Lexer(std::unique_ptr<std::istream>&& stream)
	: m_stream(std::move(stream))
{
}

Token Lexer::TokenizeNext()
{
	TrashLeadingWhiteSpace();

	char first = m_stream->get();
	if (m_stream->gcount() == 0 || !HasMoreToLex())
	{
		return Token(PrimativeToken::EndOfFile);
	}

	if (ShouldTryTokenizeOperator(first))
	{
		return TokenizeOperatorOrNegNumber(first);
	}
	else if (isdigit(first))
	{
		return TokenizeNumber(first);
	}
	
	return TokenizeKeywordOrLocation(first);
}

Token Lexer::TokenizeKeywordOrLocation(char first)
{
	std::string tokenStr = GetStringUntilPredicateNoLongerApplies(first, [](char c) { return isalnum(c);});
	if (tokenStr == "skip")
	{
		return Token(PrimativeToken::Skip);
	}
	if (tokenStr == "true")
	{
		return Token(PrimativeToken::True);
	}
	if (tokenStr == "false")
	{
		return Token(PrimativeToken::False);
	}
	if (tokenStr == "if")
	{
		return Token(PrimativeToken::If);
	}
	if (tokenStr == "then")
	{
		return Token(PrimativeToken::Then);
	}
	if (tokenStr == "else")
	{
		return Token(PrimativeToken::Else);
	}
	if (tokenStr == "while")
	{
		return Token(PrimativeToken::While);
	}
	
	return Token(PrimativeToken::Location, tokenStr);
}

Token Lexer::TokenizeNumber(char first)
{
	std::string next = GetStringUntilPredicateNoLongerApplies(first, [](char c) { return isdigit(c); });
	return Token(PrimativeToken::Number, next);
}

Token Lexer::TokenizeOperatorOrNegNumber(char first)
{
	char next;
	switch (first)
	{
	case ('('):
		return Token(PrimativeToken::LParen);
	case (')'):
		return Token(PrimativeToken::RParen);
	case ('+'):
		return Token(PrimativeToken::Plus);
	case ('-'):
		next = m_stream->peek();
		if (isdigit(next))
		{
			m_stream->get();
			Token token = TokenizeNumber(next);
			return Token(PrimativeToken::Number, "-" + token.UseValue());
		}
		return Token(PrimativeToken::Minus);
	case ('*'):
		return Token(PrimativeToken::Times);
	case ('='):
		return Token(PrimativeToken::Equal);
	case ('<'):
		next = m_stream->peek();
		if (next != '=')
		{
			throw LexException("Invalid use of '<'. Expected Less then or equal <= ");
		}
		m_stream->get();
		return Token(PrimativeToken::LEqual);
	case ('!'):
		return Token(PrimativeToken::Not);
	case ('&'):
		return Token(PrimativeToken::And);
	case ('|'):
		return Token(PrimativeToken::Or);
	case (':'):
		next = m_stream->peek();
		if (next != '=')
		{
			throw LexException("Invalid use of ':'. Expected assignment := ");
		}
		m_stream->get();
		return Token(PrimativeToken::Assign);
	}

	throw LexException("Invalid character encountered while attempting to tokenize operator!");
}


void Lexer::TrashLeadingWhiteSpace()
{
	GetStringUntilPredicateNoLongerApplies(' ', [](char c) { return isspace(c); });
}

bool Lexer::HasMoreToLex()
{
	return !m_stream->eof();
}

bool Lexer::ShouldTryTokenizeOperator(char first)
{
	std::string validOps("()+-*=<!&|:");
	return validOps.find(first) != std::string::npos;
}

std::string Lexer::GetStringUntilPredicateNoLongerApplies(char first, std::function<bool(char)>&& pred)
{
	if (!pred(first))
	{
		throw LexException("Attempt to tokenize when first char does not match predicate");
	}
	std::stringstream ss;
	ss << first;
	while (HasMoreToLex() && pred(m_stream->peek()))
	{
		char next = m_stream->get();
		if (m_stream->gcount() == 0)
		{
			break;
		}
		ss << next;
	}
	return ss.str();
}
