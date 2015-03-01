#pragma once
#include <exception>

class CustomException
    : public std::exception
{
public:
	CustomException(const char* message)
		: m_message(message)
	{}

    CustomException(std::string message)
        : m_message(message)
    {}

	virtual const char* what() const throw() override
    {
        return m_message.c_str();
    }

    virtual ~CustomException() {}

	std::string m_message;

};

class TokenException : public std::exception
{
    virtual const char* what() const throw() override
    {
        return "Invalid creation of token";
    }
};

class LexException
	: public CustomException
{
public:
	LexException(const char* message)
		: CustomException(message)
	{}
};

class ParseException
    : public CustomException
{
public:
    ParseException(const char* message)
        : CustomException(message)
    {}
};

class SemanticAnalysisException
    : public CustomException
{
public:
    SemanticAnalysisException(const char* message)
        : CustomException(message)
    {}
};

class CompilerErrorException
    : public CustomException
{
public:
    CompilerErrorException(const char* message)
        : CustomException(std::string(message) + " : This is a compiler bug")
    {}
};
