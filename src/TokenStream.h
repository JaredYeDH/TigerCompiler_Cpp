#pragma once
#include "common.h"
#include <vector>
#include "Tokens.h"
#include <iterator>

class TokenStream 
{
public:
    TokenStream(const std::vector<Token>&);
    TokenStream(std::unique_ptr<std::istream>&&);
    TokenStream(TokenStream&& other)
    	: m_tokenStream(std::move(other.m_tokenStream))
    	, m_tokenStreamIter(std::move(other.m_tokenStreamIter))
    	{}
    
    // Stateful, trashes token from stream
    Token GetNextToken();
    Token PeekNextToken() const;
private:
    std::vector<Token> m_tokenStream;
    std::vector<Token>::iterator m_tokenStreamIter;
};
