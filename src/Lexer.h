#pragma once
#include "common.h"
#include "PositionCountingStream.h"
#include "Tokens.h"
#include <istream>
#include <functional>

class Lexer
{
public:
	Lexer(std::unique_ptr<std::istream>&& stream);
	Token TokenizeNext();

private:
	PositionCountingStream m_stream;

    void TrashLeadingWhiteSpaceAndComments();

    bool TryTrashComment();

    bool TryTrashStartOfComment();

    bool TryTrashEndOfComment();

    Token TokenizeOperatorOrNegNumber(char first);

    Token TokenizeNumber(char first);

    Token TokenizeKeywordOrIdentifier(char first);

    Token TokenizeString();

    std::string GetStringUntilPredicateNoLongerApplies(
            char first,
            std::function<bool(char)>&& pred,
            const std::string& errMessage);

    bool ShouldTryTokenizeOperator(char first);

    bool HasMoreToLex();

	Lexer() = delete;

    Position m_currentStartPos {0,0};
}; 
