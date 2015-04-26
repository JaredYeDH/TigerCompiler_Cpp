#pragma once
#include "common.h"
#include "Parser.h"

class TigerCompiler
{
public:
    TigerCompiler();
    void CompileFile(const char* path);
private:
    bool HandleErrors();
    std::shared_ptr<CompileTimeErrorReporter> m_errorReporter;
    std::shared_ptr<WarningReporter> m_warningReporter;
};
