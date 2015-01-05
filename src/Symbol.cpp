#include "common.h"
#include "Symbol.h"

Symbol::Symbol(const Symbol& other)
    : m_name(other.m_name)
    , m_id(other.m_id)
{
}

Symbol::Symbol(const std::string& str, uint64_t id)
    : m_name(str)
    , m_id(id)
{
}

bool Symbol::operator==(const Symbol& other) const
{
    return other.m_id == m_id;
}

bool Symbol::operator!=(const Symbol& other) const
{
    return other.m_id != m_id;
}

bool Symbol::operator<(const Symbol& other) const
{
    return m_id < other.m_id;
}

const std::string& Symbol::UseName() const
{
    return m_name;
}

uint64_t Symbol::Hash() const
{
    return m_id;
}


uint64_t SymbolFactory::m_nextId = 0;
std::unordered_map<std::string, uint64_t> SymbolFactory::m_symbolMap;

Symbol SymbolFactory::GenerateSymbol(const std::string& name)
{
    if (name.size() == 0)
    {
        throw CompilerErrorException("Attempt to create a symbol with an empty string. This is a compiler bug. This should have been an optional symbol.");
    }
    auto iter = m_symbolMap.find(name);
    if (iter == end(m_symbolMap))
    {
        m_symbolMap[name] = m_nextId++;
    }

    return Symbol(name, m_symbolMap[name]);
}
