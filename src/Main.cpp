#include "Compiler.h"

int main(const int argc, char** argv)
{
    TigerCompiler compiler;
    compiler.CompileFile(argv[1]);
}
