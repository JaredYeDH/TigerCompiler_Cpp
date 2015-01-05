#pragma once
#include "common.h"
#include <string>
#include <unordered_map>

class SymbolFactory;

class Symbol
{
public:
    Symbol(const Symbol& other);
    bool operator==(const Symbol& other) const;
    bool operator!=(const Symbol& other) const;
    bool operator<(const Symbol& other) const;
    const std::string& UseName() const;
    uint64_t Hash() const;

private:
    Symbol() = delete;
    Symbol(const std::string& str, uint64_t id);

    std::string m_name;
    uint64_t m_id;
    friend class SymbolFactory;
};

class SymbolFactory
{
public:
    static Symbol GenerateSymbol(const std::string& name);
private:
    static uint64_t m_nextId;
    static std::unordered_map<std::string, uint64_t> m_symbolMap;
};
