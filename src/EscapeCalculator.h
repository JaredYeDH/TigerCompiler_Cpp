#pragma once
#include "Symbol.h"

class IEscapeCalculator
{
public:
    virtual void IncreaseDepth() = 0;
    virtual void DecreaseDepth() = 0;
    virtual void EscapeIfNecessary(const Symbol& symbol) = 0;
    virtual void TrackDecl(const Symbol& symbol, bool* escape) = 0;
};

namespace EscapeCalculatorFactory
{
    std::shared_ptr<IEscapeCalculator> MakeEscapeCalculator();
}
