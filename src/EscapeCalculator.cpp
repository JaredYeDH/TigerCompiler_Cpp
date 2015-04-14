#include "EscapeCalculator.h"
#include "SymbolTable.h"

typedef SymbolTable<std::pair<uint32_t, bool*>> EscapeTable;

class EscapeCalculator
    : public IEscapeCalculator
{
public:
    EscapeCalculator()
    {
        m_escapeTable.BeginScope();
    }

    ~EscapeCalculator()
    {
        m_escapeTable.EndScope();
    }

    void IncreaseDepth() override;
    void DecreaseDepth() override;
    void EscapeIfNecessary(const Symbol& symbol) override;
    void TrackDecl(const Symbol& symbol, bool* escape) override;
private:
    EscapeTable m_escapeTable;
    uint32_t m_depth = 0;
};

void EscapeCalculator::IncreaseDepth()
{
    m_escapeTable.BeginScope();
    m_depth++;
}

void EscapeCalculator::DecreaseDepth()
{
    m_escapeTable.EndScope();
    m_depth--;
}

void EscapeCalculator::EscapeIfNecessary(const Symbol& symbol)
{
    auto var = m_escapeTable.LookUp(symbol);
    if (!var)
    {
        throw CompilerErrorException("Simple vars should be entered into escape table");
    }

    if (var->first < m_depth)
    {
        *(var->second) = true;
    }
}

void EscapeCalculator::TrackDecl(const Symbol& symbol, bool* escape)
{
    bool shadowed;
    *escape = false;
    m_escapeTable.Insert(symbol, std::make_pair(m_depth, escape), shadowed);
}

namespace EscapeCalculatorFactory
{
    std::shared_ptr<IEscapeCalculator> MakeEscapeCalculator()
    {
        return std::make_shared<EscapeCalculator>();
    }
}
