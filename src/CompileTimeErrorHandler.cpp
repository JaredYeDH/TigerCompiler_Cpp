#include "CompileTimeErrorHandler.h"
#include <iostream>

void CompileTimeErrorReporter::AddError(const Error& error)
{
    m_errors.push_back(error);
}

void CompileTimeErrorReporter::ReportAllGivenErrors() const
{
    for (const Error& err : m_errors)
    {
        std::cerr << err.What() << " occured at " << err.Where() << "\n";
    }
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
            std::cerr << "Warning: " << warning.What() << " occured at " << warning.Where() << "\n";
        }
    }
}

bool WarningReporter::HasAnyErrors() const
{
    return m_warnings.empty();
}
