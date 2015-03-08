#pragma once

#include "common.h"
#include "Position.h"

#include <sstream>

typedef std::string ErrorMsg;

enum class ErrorType : uint8_t
{
    SyntaxError,
    TypeError
};

class Error
{
public:
    Error(ErrorType type, const Position& where, const ErrorMsg& what)
        : m_where(where)
    {
        std::stringstream ss;
        ss << ErrorTypeToString(type) << " " << what;
        m_what = ss.str();
    }

    const Position& Where() const
    {
        return m_where;
    }

    const ErrorMsg& What() const
    {
        return m_what;
    }

private:
    std::string ErrorTypeToString(ErrorType type)
    {
        switch(type)
        {
        case ErrorType::SyntaxError:
            return "Syntax Error: ";
        case ErrorType::TypeError:
            return "Type Error: ";
        default:
            return "Error: ";
        }
    }
    Position m_where;
    ErrorMsg m_what;
};

class CompileTimeErrorReporter
{
public:
    void AddError(const Error& error);

    void ReportAllGivenErrors() const;

    bool HasAnyErrors() const
    {
        return m_errors.empty();
    }

protected:
    std::vector<Error> m_errors;
};

enum class WarningLevel : uint8_t
{
    Low,
    Medium,
    High
};

class Warning
{
public:
    Warning(const Position& where, const ErrorMsg& what, WarningLevel sev)
        : m_where(where)
        , m_what(what)
        , m_level(sev)
    {
        // empty
    }

    const Position& Where() const
    {
        return m_where;
    }

    const ErrorMsg& What() const
    {
        return m_what;
    }

    WarningLevel Level() const
    {
        return m_level;
    }

private:
    Position m_where;
    ErrorMsg m_what;
    WarningLevel m_level;
};


class WarningReporter
{
public:
    void AddWarning(const Warning& warning);
    void ReportAllWarningsAtOrBelowLevel(WarningLevel level) const;
    bool HasAnyErrors() const;
private:
    std::vector<Warning> m_warnings;
};
