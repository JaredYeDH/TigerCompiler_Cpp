#pragma once

#include "common.h"
#include "Position.h"
#include "ErrorCodes.h"

#include <sstream>

typedef std::string SupplementalErrorMsg;

class Error
{
public:
    Error(ErrorCode code, const Position& where, const SupplementalErrorMsg& what)
        : m_where(where)
        , m_what(what)
        , m_errorCode(code)
    {
    }

    const ErrorCode& UseErrorCode() const
    {
        return m_errorCode;
    }

    const std::string& Report() const
    {
        if (m_report.empty())
        {
            std::stringstream ss;
            ss << ErrorCodes::GetAssociatedMessage(m_errorCode) << " occured at " << m_where << " with " << m_what;
            m_report = ss.str();
        }
        return m_report;
    }

private:
    Position m_where;
    SupplementalErrorMsg m_what;
    ErrorCode m_errorCode;
    mutable std::string m_report;
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
    Warning(const Position& where, const std::string& what, WarningLevel sev)
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

    const std::string& What() const
    {
        return m_what;
    }

    WarningLevel Level() const
    {
        return m_level;
    }

private:
    Position m_where;
    std::string m_what;
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
