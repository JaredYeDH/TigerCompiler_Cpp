#include "Compiler.h"

TigerCompiler::TigerCompiler()
    : m_errorReporter(std::make_shared<CompileTimeErrorReporter>())
    , m_warningReporter(std::make_shared<WarningReporter>())
{
}

void TigerCompiler::CompileFile(const char* file)
{
    Parser parser = Parser::CreateParserForFile(file, m_errorReporter, m_warningReporter);
    
    auto program = parser.Parse();
    if (HandleErrors()) { return; }
    program->CalculateEscapes();
    if (HandleErrors()) { return; }
    program->TypeCheck();
    if (HandleErrors()) { return; }
}

bool TigerCompiler::HandleErrors()
{
    if (m_errorReporter->HasAnyErrors())
    {
        m_errorReporter->ReportAllGivenErrors();
        return true;
    }
    return false;
}
