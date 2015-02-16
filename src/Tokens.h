#pragma once
#include "common.h"
#include "Position.h"
#include <string>

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
    Token(PrimativeToken primToken, Position position) throw(TokenException);
    Token(PrimativeToken primToken, const std::string& value, Position position) throw(TokenException);

    PrimativeToken GetTokenType() const;
    const std::string& UseValue() const;
    const Position& UsePosition() const;

private:
    bool HasValue() const;
    PrimativeToken m_type;
    std::string m_value;
    Position m_position;

    void ThrowIfInproperInit() throw(TokenException);
};

