#pragma once
#include "Symbol.h"

namespace Temps
{

using Label = Symbol;

class LocalTemp
{
public:
    LocalTemp(uint64_t id)
        : m_id(id)
    { /* empty */ }

    bool operator==(const LocalTemp& other) const
    {
        return m_id == other.m_id;
    }

    bool operator!=(const LocalTemp& other) const
    {
        return !(*this == other);
    }

private:
    uint64_t m_id;
};

class ITempFactory
{
public:
    virtual LocalTemp MakeTempVar() const = 0;
    virtual Label MakeLabel() const = 0;
    virtual Label MakeNamedLabel(const char* label) const = 0;
};

const ITempFactory& UseTempFactory();

}
