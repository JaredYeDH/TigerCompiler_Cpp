#include "CompileTimeErrorHandler.h"
#include <iostream>

using namespace std;

void CompileTimeErrorReporter::AddError(const Error& error)
{
    m_errors.push_back(error);
}

void CompileTimeErrorReporter::ReportAllGivenErrors() const
{
    for (const Error& err : m_errors)
    {
        cerr << "Error: " << err.Report() << "\n";
    }
}

bool CompileTimeErrorReporter::ContainsErrorCode(const ErrorCode& err) const
{
    const auto it = find_if(begin(m_errors), end(m_errors), [&err](const Error& other) { return err == other.UseErrorCode(); });
    return it != end(m_errors);
}

void WarningReporter::AddWarning(const Warning& warning)
{
    m_warnings.push_back(warning);
}

void WarningReporter::ReportAllWarningsAtOrBelowLevel(WarningLevel level) const
{
    for (const Warning& warning : m_warnings)
    {
        if (warning.Level() <= level)
        {
            cerr << "Warning: " << warning.What() << " occured at " << warning.Where() << "\n";
        }
    }
}

bool WarningReporter::HasAnyErrors() const
{
    return m_warnings.empty();
}
