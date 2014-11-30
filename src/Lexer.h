#pragma once
#include "common.h"
#include "Tokens.h"
#include <istream>
#include <exception>
#include <functional>

class LexException
	: std::exception
{
public:
	LexException(const char* message)
		: m_message(message)
	{}

	virtual const char* what() const throw() override
    {
        return m_message.c_str();
    }

private:
	std::string m_message;
};

class Lexer
{
public:
	Lexer(std::unique_ptr<std::istream>&& stream);
	Token TokenizeNext();

private:
	std::unique_ptr<std::istream> m_stream;

    void TrashLeadingWhiteSpaceAndComments();

    bool TryTrashComment();

    bool TryTrashStartOfComment();

    bool TryTrashEndOfComment();

    Token TokenizeOperatorOrNegNumber(char first);

    Token TokenizeNumber(char first);

    Token TokenizeKeywordOrIdentifier(char first);

    std::string GetStringUntilPredicateNoLongerApplies(char first, std::function<bool(char)>&& pred);

    bool ShouldTryTokenizeOperator(char first);

    bool HasMoreToLex();

	Lexer() = delete;
}; 
