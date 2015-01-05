#include "Lexer.h"
#include <sstream>

Lexer::Lexer(std::unique_ptr<std::istream>&& stream)
	: m_stream(std::move(stream))
{
}

Token Lexer::TokenizeNext()
{
	TrashLeadingWhiteSpaceAndComments();

	char first = m_stream->get();
	if (m_stream->gcount() == 0)
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
	else if (first == '"')
	{
		return TokenizeString();
	}
	
	return TokenizeKeywordOrIdentifier(first);
}

Token Lexer::TokenizeKeywordOrIdentifier(char first)
{
	std::string tokenStr = GetStringUntilPredicateNoLongerApplies(first, [](char c) { return isalnum(c) || c == '_';});
	
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
	if (tokenStr == "for")
	{
		return Token(PrimativeToken::For);
	}
	if (tokenStr == "to")
	{
		return Token(PrimativeToken::To);
	}
	if (tokenStr == "do")
	{
		return Token(PrimativeToken::Do);
	}
	if (tokenStr == "let")
	{
		return Token(PrimativeToken::Let);
	}
	if (tokenStr == "in")
	{
		return Token(PrimativeToken::In);
	}
	if (tokenStr == "end")
	{
		return Token(PrimativeToken::End);
	}
	if (tokenStr == "of")
	{
		return Token(PrimativeToken::Of);
	}
	if (tokenStr == "break")
	{
		return Token(PrimativeToken::Break);
	}
	if (tokenStr == "nil")
	{
		return Token(PrimativeToken::Nil);
	}
	if (tokenStr == "function")
	{
		return Token(PrimativeToken::Function);
	}
	if (tokenStr == "var")
	{
		return Token(PrimativeToken::Var);
	}
	if (tokenStr == "type")
	{
		return Token(PrimativeToken::Type);
	}
	if (tokenStr == "import")
	{
		return Token(PrimativeToken::Import);
	}
	if (tokenStr == "primative")
	{
		return Token(PrimativeToken::Primative);
	}
	if (tokenStr == "array")
	{
		return Token(PrimativeToken::Array);
	}

	return Token(PrimativeToken::Identifier, tokenStr);
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
	case (','):
		return Token(PrimativeToken::Comma);
	case (':'):
		next = m_stream->peek();
		if (next == '=')
		{
			m_stream->get();
			return Token(PrimativeToken::Assign);
		}
		return Token(PrimativeToken::Colon);
	case (';'):
		return Token(PrimativeToken::Semi);
	case ('('):
		return Token(PrimativeToken::LParen);
	case (')'):
		return Token(PrimativeToken::RParen);
	case ('['):
		return Token(PrimativeToken::LBracket);
	case (']'):
		return Token(PrimativeToken::RBracket);
	case ('{'):
		return Token(PrimativeToken::LBrace);
	case ('}'):
		return Token(PrimativeToken::RBrace);
	case ('.'):
		return Token(PrimativeToken::Period);
	case ('+'):
		return Token(PrimativeToken::Plus);
	case ('-'):
		return Token(PrimativeToken::Minus);
	case ('*'):
		return Token(PrimativeToken::Times);
	case ('/'):
		return Token(PrimativeToken::Div);
	case ('='):
		return Token(PrimativeToken::Equal);
	case ('<'):
		next = m_stream->peek();
		if (next == '=')
		{
			m_stream->get();
			return Token(PrimativeToken::LEqual);
		}
		if (next == '>')
		{
			m_stream->get();
			return Token(PrimativeToken::NotEqual);
		}
		return Token(PrimativeToken::LessThan);
	case ('>'):
		next = m_stream->peek();
		if (next == '=')
		{
			m_stream->get();
			return Token(PrimativeToken::GEqual);
		}
		return Token(PrimativeToken::GreaterThan);
	case ('&'):
		return Token(PrimativeToken::And);
	case ('|'):
		return Token(PrimativeToken::Or);
	}

	throw LexException("Invalid character encountered while attempting to tokenize operator!");
}

Token Lexer::TokenizeString()
{
	std::string value = GetStringUntilPredicateNoLongerApplies(' ' , [](char c) { return c != '"'; });
	// Trash closing quote
	m_stream->get();
	return Token(PrimativeToken::StringLit, value.substr(1, value.length()));
}

void Lexer::TrashLeadingWhiteSpaceAndComments()
{
	bool somethingWasTrashed = false;
	while (true)
	{
		somethingWasTrashed = GetStringUntilPredicateNoLongerApplies(' ', [](char c) { return isspace(c); }).length() > 1;
		somethingWasTrashed |= TryTrashComment();
		if (!somethingWasTrashed)
		{
			return;
		}
	}
}

bool Lexer::TryTrashComment()
{
	int depth = TryTrashStartOfComment() ? 1 : 0;

	if (depth == 0)
	{
		return false;
	}

	while (depth > 0 && HasMoreToLex())
	{
		if (TryTrashStartOfComment())
		{
			depth++;
		}

		if (TryTrashEndOfComment())
		{
			depth--;
		}
		else
		{
			m_stream->get();
		}
	}

	if (depth > 0)
	{
		throw LexException("Un-closed comment");
	}
	return true;
}

bool Lexer::TryTrashStartOfComment()
{
	if (m_stream->peek() == '/')
	{
		m_stream->get();
		if (m_stream->peek() != '*')
		{
			m_stream->unget();
			return false;
		}
		else
		{
			m_stream->get();
			return true;
		}
	}
	return false;
}

bool Lexer::TryTrashEndOfComment()
{
	if (m_stream->peek() == '*')
	{
		m_stream->get();
		if (m_stream->peek() != '/')
		{
			m_stream->unget();
			return false;
		}
		else
		{
			m_stream->get();
			return true;
		}
	}
	return false;
}

bool Lexer::HasMoreToLex()
{
	return !m_stream->eof();
}

bool Lexer::ShouldTryTokenizeOperator(char first)
{
	std::string validOps(",:;()[]{}.+-*/=<>&|");
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
