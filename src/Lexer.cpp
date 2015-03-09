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

    m_currentStartPos = m_stream.GetCurrentPosition();
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
		return Token(PrimativeToken::If, m_currentStartPos);
	}
	if (tokenStr == "then")
	{
		return Token(PrimativeToken::Then, m_currentStartPos);
	}
	if (tokenStr == "else")
	{
		return Token(PrimativeToken::Else, m_currentStartPos);
	}
	if (tokenStr == "while")
	{
		return Token(PrimativeToken::While, m_currentStartPos);
	}
	if (tokenStr == "for")
	{
		return Token(PrimativeToken::For, m_currentStartPos);
	}
	if (tokenStr == "to")
	{
		return Token(PrimativeToken::To, m_currentStartPos);
	}
	if (tokenStr == "do")
	{
		return Token(PrimativeToken::Do, m_currentStartPos);
	}
	if (tokenStr == "let")
	{
		return Token(PrimativeToken::Let, m_currentStartPos);
	}
	if (tokenStr == "in")
	{
		return Token(PrimativeToken::In, m_currentStartPos);
	}
	if (tokenStr == "end")
	{
		return Token(PrimativeToken::End, m_currentStartPos);
	}
	if (tokenStr == "of")
	{
		return Token(PrimativeToken::Of, m_currentStartPos);
	}
	if (tokenStr == "break")
	{
		return Token(PrimativeToken::Break, m_currentStartPos);
	}
	if (tokenStr == "nil")
	{
		return Token(PrimativeToken::Nil, m_currentStartPos);
	}
	if (tokenStr == "function")
	{
		return Token(PrimativeToken::Function, m_currentStartPos);
	}
	if (tokenStr == "var")
	{
		return Token(PrimativeToken::Var, m_currentStartPos);
	}
	if (tokenStr == "type")
	{
		return Token(PrimativeToken::Type, m_currentStartPos);
	}
	if (tokenStr == "import")
	{
		return Token(PrimativeToken::Import, m_currentStartPos);
	}
	if (tokenStr == "primative")
	{
		return Token(PrimativeToken::Primative, m_currentStartPos);
	}
	if (tokenStr == "array")
	{
		return Token(PrimativeToken::Array, m_currentStartPos);
	}

	return Token(PrimativeToken::Identifier, tokenStr, m_currentStartPos);
}

Token Lexer::TokenizeNumber(char first)
{
	std::string next = GetStringUntilPredicateNoLongerApplies(first, [](char c) { return isdigit(c); }, "number");
	return Token(PrimativeToken::Number, next, m_currentStartPos);
}

Token Lexer::TokenizeOperatorOrNegNumber(char first)
{
	char next;
	switch (first)
	{
	case (','):
		return Token(PrimativeToken::Comma, m_currentStartPos);
	case (':'):
		next = m_stream.peek();
		if (next == '=')
		{
			m_stream.get();
			return Token(PrimativeToken::Assign, m_currentStartPos);
		}
		return Token(PrimativeToken::Colon, m_currentStartPos);
	case (';'):
		return Token(PrimativeToken::Semi, m_currentStartPos);
	case ('('):
		return Token(PrimativeToken::LParen, m_currentStartPos);
	case (')'):
		return Token(PrimativeToken::RParen, m_currentStartPos);
	case ('['):
		return Token(PrimativeToken::LBracket, m_currentStartPos);
	case (']'):
		return Token(PrimativeToken::RBracket, m_currentStartPos);
	case ('{'):
		return Token(PrimativeToken::LBrace, m_currentStartPos);
	case ('}'):
		return Token(PrimativeToken::RBrace, m_currentStartPos);
	case ('.'):
		return Token(PrimativeToken::Period, m_currentStartPos);
	case ('+'):
		return Token(PrimativeToken::Plus, m_currentStartPos);
	case ('-'):
		return Token(PrimativeToken::Minus, m_currentStartPos);
	case ('*'):
		return Token(PrimativeToken::Times, m_currentStartPos);
	case ('/'):
		return Token(PrimativeToken::Div, m_currentStartPos);
	case ('='):
		return Token(PrimativeToken::Equal, m_currentStartPos);
	case ('<'):
		next = m_stream.peek();
		if (next == '=')
		{
			m_stream.get();
			return Token(PrimativeToken::LEqual, m_currentStartPos);
		}
		if (next == '>')
		{
			m_stream.get();
			return Token(PrimativeToken::NotEqual, m_currentStartPos);
		}
		return Token(PrimativeToken::LessThan, m_currentStartPos);
	case ('>'):
		next = m_stream.peek();
		if (next == '=')
		{
			m_stream.get();
			return Token(PrimativeToken::GEqual, m_currentStartPos);
		}
		return Token(PrimativeToken::GreaterThan, m_currentStartPos);
	case ('&'):
		return Token(PrimativeToken::And, m_currentStartPos);
	case ('|'):
		return Token(PrimativeToken::Or, m_currentStartPos);
	}

    stringstream ss;
    ss << "Invalid character (" << first << ") encountered while attempting to tokenize operator.\n";
    ss << "At line:" << m_currentStartPos.GetRow();
    ss << " column:" << m_currentStartPos.GetColumn() << "\n";
	throw LexException(ss.str().c_str());
}

Token Lexer::TokenizeString()
{
	std::string value = GetStringUntilPredicateNoLongerApplies(' ' , [](char c) { return c != '"'; }, "string literal");
	// Trash closing quote
	m_stream.get();
	return Token(PrimativeToken::StringLit, value.substr(1, value.length()), m_currentStartPos);
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

