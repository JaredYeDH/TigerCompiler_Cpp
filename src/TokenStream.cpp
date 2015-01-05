#include "TokenStream.h"
#include "Lexer.h"

using namespace std;
TokenStream::TokenStream(const vector<Token>& tokens)
    : m_tokenStream(tokens)
    , m_tokenStreamIter(begin(m_tokenStream))
{
}

TokenStream::TokenStream(unique_ptr<istream>&& instream)
{
    Lexer lexer(move(instream));
    vector<Token> tokens;
    do
    {
        tokens.push_back(lexer.TokenizeNext());
    } while(tokens.back().GetTokenType() != PrimativeToken::EndOfFile);
    m_tokenStream = move(tokens);
    m_tokenStreamIter = begin(m_tokenStream);
}

Token TokenStream::GetNextToken()
{
    Token token = PeekNextToken();
    if (m_tokenStreamIter != end(m_tokenStream))
    {
        m_tokenStreamIter++;
    }
    return token;
}

Token TokenStream::PeekNextToken() const
{
    if (m_tokenStreamIter == end(m_tokenStream))
    {
        return Token{PrimativeToken::EndOfFile};
    }
    return *m_tokenStreamIter;
}
