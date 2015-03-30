#include "common.h"
#include "PositionCountingStream.h"
#include "Parser.h"
#include <sstream>
#include <iostream>

int main(const int argc, char** argv)
{
    std::shared_ptr<CompileTimeErrorReporter> errorReporter = std::make_shared<CompileTimeErrorReporter>();
    std::shared_ptr<WarningReporter> warningReporter = std::make_shared<WarningReporter>();
    Parser parser = Parser::CreateParserForFile(argv[1], errorReporter, warningReporter);
    auto prog = parser.Parse();
    std::cout << prog->DumpAST();
}
