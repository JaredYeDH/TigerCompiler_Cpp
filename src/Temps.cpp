#include "Temps.h"
#include <sstream>

using namespace Temps;

class TempFactory
    : public ITempFactory
{
public:
    LocalTemp MakeTempVar() const override;
    Label MakeLabel() const override;
    Label MakeNamedLabel(const char* label) const override;
private:
    mutable uint64_t m_nextTempId = 0;
    mutable uint64_t m_nextLabelId = 0;
    std::string m_labelPrefix = "__TIGER_LABEL_";
    std::string m_labelPostfix = "__";
};

LocalTemp TempFactory::MakeTempVar() const
{
    return LocalTemp{m_nextTempId++};
}

Label TempFactory::MakeLabel() const
{
    std::stringstream label;
    label << m_labelPrefix << m_nextLabelId++ << m_labelPostfix;
    return SymbolFactory::GenerateSymbol(label.str());
}

Label TempFactory::MakeNamedLabel(const char* label) const
{
    return SymbolFactory::GenerateSymbol(label);
}

const ITempFactory& Temps::UseTempFactory()
{
    static TempFactory tempFactory;
    return tempFactory;
}
