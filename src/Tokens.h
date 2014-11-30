#pragma once
#include <string>
#include <exception>

class TokenException : public std::exception
{
    virtual const char* what() const throw() override
    {
        return "Invalid creation of token";
    }
};

enum class PrimativeToken : unsigned int 
{
    EndOfFile,
    LParen,
    RParen,
    Plus,
    Minus,
    Times,
    Equal,
    LEqual,
    Not,
    And,
    Or,
    Assign,

    Skip,
    True,
    False,
    If,
    Then,
    Else,
    While,
    
    Number,
    Location
};

class Token
{
public:
    Token(PrimativeToken primToken) throw(TokenException);
    Token(PrimativeToken primToken, const std::string& value) throw(TokenException);

    bool HasValue() const;
    PrimativeToken GetTokenType() const;
    const std::string& UseValue() const;

private:
    PrimativeToken m_type;
    std::string m_value;
    void ThrowIfInproperInit() throw(TokenException);};

