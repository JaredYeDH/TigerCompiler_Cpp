#include "Lexer.h"
#include <sstream>

using namespace std;

Lexer::Lexer(std::unique_ptr<std::istream>&& stream)
	: m_stream(std::move(stream))
{
}

Token Lexer::TokenizeNext()
{
	TrashLeadingWhiteSpaceAndComments();
    
    if (!HasMoreToLex())
    {
        return Token(PrimativeToken::EndOfFile, m_stream.GetCurrentPosition());
    }

	char first = m_stream.get();

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
	std::string tokenStr = GetStringUntilPredicateNoLongerApplies(first, [](char c) { return isalnum(c) || c == '_';}, "Keyword or identifier");
	
	if (tokenStr == "if")
	{
		return Token(PrimativeToken::If, m_stream.GetCurrentPosition());
	}
	if (tokenStr == "then")
	{
		return Token(PrimativeToken::Then, m_stream.GetCurrentPosition());
	}
	if (tokenStr == "else")
	{
		return Token(PrimativeToken::Else, m_stream.GetCurrentPosition());
	}
	if (tokenStr == "while")
	{
		return Token(PrimativeToken::While, m_stream.GetCurrentPosition());
	}
	if (tokenStr == "for")
	{
		return Token(PrimativeToken::For, m_stream.GetCurrentPosition());
	}
	if (tokenStr == "to")
	{
		return Token(PrimativeToken::To, m_stream.GetCurrentPosition());
	}
	if (tokenStr == "do")
	{
		return Token(PrimativeToken::Do, m_stream.GetCurrentPosition());
	}
	if (tokenStr == "let")
	{
		return Token(PrimativeToken::Let, m_stream.GetCurrentPosition());
	}
	if (tokenStr == "in")
	{
		return Token(PrimativeToken::In, m_stream.GetCurrentPosition());
	}
	if (tokenStr == "end")
	{
		return Token(PrimativeToken::End, m_stream.GetCurrentPosition());
	}
	if (tokenStr == "of")
	{
		return Token(PrimativeToken::Of, m_stream.GetCurrentPosition());
	}
	if (tokenStr == "break")
	{
		return Token(PrimativeToken::Break, m_stream.GetCurrentPosition());
	}
	if (tokenStr == "nil")
	{
		return Token(PrimativeToken::Nil, m_stream.GetCurrentPosition());
	}
	if (tokenStr == "function")
	{
		return Token(PrimativeToken::Function, m_stream.GetCurrentPosition());
	}
	if (tokenStr == "var")
	{
		return Token(PrimativeToken::Var, m_stream.GetCurrentPosition());
	}
	if (tokenStr == "type")
	{
		return Token(PrimativeToken::Type, m_stream.GetCurrentPosition());
	}
	if (tokenStr == "import")
	{
		return Token(PrimativeToken::Import, m_stream.GetCurrentPosition());
	}
	if (tokenStr == "primative")
	{
		return Token(PrimativeToken::Primative, m_stream.GetCurrentPosition());
	}
	if (tokenStr == "array")
	{
		return Token(PrimativeToken::Array, m_stream.GetCurrentPosition());
	}

	return Token(PrimativeToken::Identifier, tokenStr, m_stream.GetCurrentPosition());
}

Token Lexer::TokenizeNumber(char first)
{
	std::string next = GetStringUntilPredicateNoLongerApplies(first, [](char c) { return isdigit(c); }, "number");
	return Token(PrimativeToken::Number, next, m_stream.GetCurrentPosition());
}

Token Lexer::TokenizeOperatorOrNegNumber(char first)
{
	char next;
	switch (first)
	{
	case (','):
		return Token(PrimativeToken::Comma, m_stream.GetCurrentPosition());
	case (':'):
		next = m_stream.peek();
		if (next == '=')
		{
			m_stream.get();
			return Token(PrimativeToken::Assign, m_stream.GetCurrentPosition());
		}
		return Token(PrimativeToken::Colon, m_stream.GetCurrentPosition());
	case (';'):
		return Token(PrimativeToken::Semi, m_stream.GetCurrentPosition());
	case ('('):
		return Token(PrimativeToken::LParen, m_stream.GetCurrentPosition());
	case (')'):
		return Token(PrimativeToken::RParen, m_stream.GetCurrentPosition());
	case ('['):
		return Token(PrimativeToken::LBracket, m_stream.GetCurrentPosition());
	case (']'):
		return Token(PrimativeToken::RBracket, m_stream.GetCurrentPosition());
	case ('{'):
		return Token(PrimativeToken::LBrace, m_stream.GetCurrentPosition());
	case ('}'):
		return Token(PrimativeToken::RBrace, m_stream.GetCurrentPosition());
	case ('.'):
		return Token(PrimativeToken::Period, m_stream.GetCurrentPosition());
	case ('+'):
		return Token(PrimativeToken::Plus, m_stream.GetCurrentPosition());
	case ('-'):
		return Token(PrimativeToken::Minus, m_stream.GetCurrentPosition());
	case ('*'):
		return Token(PrimativeToken::Times, m_stream.GetCurrentPosition());
	case ('/'):
		return Token(PrimativeToken::Div, m_stream.GetCurrentPosition());
	case ('='):
		return Token(PrimativeToken::Equal, m_stream.GetCurrentPosition());
	case ('<'):
		next = m_stream.peek();
		if (next == '=')
		{
			m_stream.get();
			return Token(PrimativeToken::LEqual, m_stream.GetCurrentPosition());
		}
		if (next == '>')
		{
			m_stream.get();
			return Token(PrimativeToken::NotEqual, m_stream.GetCurrentPosition());
		}
		return Token(PrimativeToken::LessThan, m_stream.GetCurrentPosition());
	case ('>'):
		next = m_stream.peek();
		if (next == '=')
		{
			m_stream.get();
			return Token(PrimativeToken::GEqual, m_stream.GetCurrentPosition());
		}
		return Token(PrimativeToken::GreaterThan, m_stream.GetCurrentPosition());
	case ('&'):
		return Token(PrimativeToken::And, m_stream.GetCurrentPosition());
	case ('|'):
		return Token(PrimativeToken::Or, m_stream.GetCurrentPosition());
	}

    stringstream ss;
    ss << "Invalid character (" << first << ") encountered while attempting to tokenize operator.\n";
    ss << "At line:" << m_stream.GetCurrentPosition().GetRow();
    ss << " column:" << m_stream.GetCurrentPosition().GetColumn() << "\n";
	throw LexException(ss.str().c_str());
}

Token Lexer::TokenizeString()
{
	std::string value = GetStringUntilPredicateNoLongerApplies(' ' , [](char c) { return c != '"'; }, "string literal");
	// Trash closing quote
	m_stream.get();
	return Token(PrimativeToken::StringLit, value.substr(1, value.length()), m_stream.GetCurrentPosition());
}

void Lexer::TrashLeadingWhiteSpaceAndComments()
{
	bool somethingWasTrashed = false;
	while (true)
	{
		somethingWasTrashed = GetStringUntilPredicateNoLongerApplies(' ', [](char c) { return isspace(c); }, "comment").length() > 1;
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
			m_stream.get();
		}
	}

	if (depth > 0)
	{
        stringstream err;
        err << "Un-closed comment at line: " << m_stream.GetCurrentPosition().GetRow();
        err << " column: " << m_stream.GetCurrentPosition().GetColumn();
		throw LexException(err.str().c_str());
	}
	return true;
}

bool Lexer::TryTrashStartOfComment()
{
	if (m_stream.peek() == '/')
	{
		if (m_stream.peekTwo() != '*')
		{
			return false;
		}
		else
		{
			m_stream.get();
            m_stream.get();
			return true;
		}
	}
	return false;
}

bool Lexer::TryTrashEndOfComment()
{
	if (m_stream.peek() == '*')
	{
		if (m_stream.peekTwo() != '/')
		{
			return false;
		}
		else
		{
			m_stream.get();
            m_stream.get();
			return true;
		}
	}
	return false;
}

bool Lexer::HasMoreToLex()
{
	return !m_stream.eof();
}

bool Lexer::ShouldTryTokenizeOperator(char first)
{
	std::string validOps(",:;()[]{}.+-*/=<>&|");
	return validOps.find(first) != std::string::npos;
}

std::string Lexer::GetStringUntilPredicateNoLongerApplies(
        char first,
        std::function<bool(char)>&& pred,
        const string& errMessage)
{
	if (!pred(first))
	{
        stringstream err;
        err << "Encountered " << first << " when attempting to tokenize a " << errMessage;
        err << "\nOccured at line: " << m_stream.GetCurrentPosition().GetRow();
        err << " column: " << m_stream.GetCurrentPosition().GetColumn();
		throw LexException(err.str().c_str());
	}
	std::stringstream ss;
	ss << first;
	while (HasMoreToLex() && pred(m_stream.peek()))
	{
		char next = m_stream.get();
		ss << next;
	}
	return ss.str();
}

