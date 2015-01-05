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
    // symbols
    EndOfFile,
    Comma,
    Colon,
    Semi,
    LParen,
    RParen,
    LBracket,
    RBracket,
    LBrace,
    RBrace,
    Period,
    Plus,
    Minus,
    Times,
    Div,
    Equal,
    NotEqual,
    LessThan,
    LEqual,
    GreaterThan,
    GEqual,
    And,
    Or,
    Assign,

    // Keywords
    If,
    Then,
    Else,
    While,
    For,
    To,
    Do,
    Let,
    In,
    End,
    Of,
    Break,
    Nil,
    Function,
    Var,
    Type,
    Import,
    Primative,
    Array,

    // Things with values   
    Number,
    Identifier,
    StringLit
};

class Token
{
public:
    Token(PrimativeToken primToken) throw(TokenException);
    Token(PrimativeToken primToken, const std::string& value) throw(TokenException);

    PrimativeToken GetTokenType() const;
    const std::string& UseValue() const;

private:
    bool HasValue() const;
    PrimativeToken m_type;
    std::string m_value;
    void ThrowIfInproperInit() throw(TokenException);};

