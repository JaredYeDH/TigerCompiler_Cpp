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
    
    // Stateful, trashes token from stream
    Token GetNextToken();
    Token PeekNextToken();
private:
    std::vector<Token> m_tokenStream;
    std::vector<Token>::iterator m_tokenStreamIter;
};
