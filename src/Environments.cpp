#include "Environments.h"

using namespace std;

void VerifyAdd(const Symbol& sym, Type ty, std::shared_ptr<TypeEnvironment>& symTable)
{
    bool shadowed;
    symTable->Insert(sym, make_shared<EnvType>(ty), shadowed);
    if (shadowed)
    {
        throw CompilerErrorException("Base Value envirionment is shadowing an entry");
    }
}

void VerifyAdd(const Symbol& sym, const std::shared_ptr<EnvEntry>& ty, std::shared_ptr<ValueEnvironment>& symTable)
{
    bool shadowed;
    symTable->Insert(sym, ty, shadowed);
    if (shadowed)
    {
        throw CompilerErrorException("Base Value envirionment is shadowing an entry");
    }
}

shared_ptr<TypeEnvironment> TypeEnvironment::GenerateBaseTypeEnvironment()
{
    auto tyEnv = make_shared<TypeEnvironment>();
    tyEnv->BeginScope();
    VerifyAdd(SymbolFactory::GenerateSymbol("int"), TypeFactory::MakeIntType(), tyEnv);
    VerifyAdd(SymbolFactory::GenerateSymbol("string"), TypeFactory::MakeStringType(), tyEnv);
    return tyEnv;
}

shared_ptr<ValueEnvironment> ValueEnvironment::GenerateBaseValueEnvironment()
{
    auto valEnv = make_shared<ValueEnvironment>();
    valEnv->BeginScope();
    auto print = make_shared<FunEntry>(vector<Type>{TypeFactory::MakeStringType()}, TypeFactory::MakeUnitType(), nullptr, Temps::UseTempFactory().MakeLabel());
    VerifyAdd(SymbolFactory::GenerateSymbol("print"), print, valEnv);
    auto flush = make_shared<FunEntry>(vector<Type>{}, TypeFactory::MakeUnitType(), nullptr, Temps::UseTempFactory().MakeLabel());
    VerifyAdd(SymbolFactory::GenerateSymbol("flush"), flush, valEnv);
    auto getchar = make_shared<FunEntry>(vector<Type>{}, TypeFactory::MakeStringType(), nullptr, Temps::UseTempFactory().MakeLabel());
    VerifyAdd(SymbolFactory::GenerateSymbol("getchar"), getchar, valEnv);
    auto ord = make_shared<FunEntry>(vector<Type>{TypeFactory::MakeStringType()}, TypeFactory::MakeIntType(), nullptr, Temps::UseTempFactory().MakeLabel());
    VerifyAdd(SymbolFactory::GenerateSymbol("ord"), ord, valEnv);
    auto chr = make_shared<FunEntry>(vector<Type>{TypeFactory::MakeIntType()}, TypeFactory::MakeStringType(), nullptr, Temps::UseTempFactory().MakeLabel());
    VerifyAdd(SymbolFactory::GenerateSymbol("chr"), chr, valEnv);
    auto size = make_shared<FunEntry>(vector<Type>{TypeFactory::MakeStringType()}, TypeFactory::MakeIntType(), nullptr, Temps::UseTempFactory().MakeLabel());
    VerifyAdd(SymbolFactory::GenerateSymbol("size"), size, valEnv);
    auto substring = make_shared<FunEntry>(vector<Type>{TypeFactory::MakeStringType(), TypeFactory::MakeIntType(), TypeFactory::MakeIntType()}, TypeFactory::MakeStringType(), nullptr, Temps::UseTempFactory().MakeLabel());
    VerifyAdd(SymbolFactory::GenerateSymbol("substring"), substring, valEnv);
    auto concat = make_shared<FunEntry>(vector<Type>{TypeFactory::MakeStringType(), TypeFactory::MakeStringType()}, TypeFactory::MakeStringType(), nullptr, Temps::UseTempFactory().MakeLabel());
    VerifyAdd(SymbolFactory::GenerateSymbol("concat"), concat, valEnv);
    auto nott = make_shared<FunEntry>(vector<Type>{TypeFactory::MakeIntType()}, TypeFactory::MakeIntType(), nullptr, Temps::UseTempFactory().MakeLabel());
    VerifyAdd(SymbolFactory::GenerateSymbol("not"), nott, valEnv);
    auto exit = make_shared<FunEntry>(vector<Type>{TypeFactory::MakeIntType()}, TypeFactory::MakeUnitType(), nullptr, Temps::UseTempFactory().MakeLabel());
    VerifyAdd(SymbolFactory::GenerateSymbol("exit"), exit, valEnv);
    return valEnv;
}
