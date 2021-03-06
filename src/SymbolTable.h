#pragma once

#include "common.h"
#include "Symbol.h"
#include <boost/optional.hpp>
#include <map>
#include <stack>
#include <vector>

template <typename Type>
struct ISymbolTable
{
    virtual void Insert(const Symbol& symbol, const Type& entry, bool& overwrite, bool isImmutable = false) = 0;
    virtual boost::optional<Type> LookUp(const Symbol& symbol, bool* isImmutable = nullptr) const = 0;
    virtual void BeginScope() = 0;
    virtual void EndScope() = 0;
    virtual ~ISymbolTable() {}
};

struct SymbolTableFactory;

template <typename Type>
class SymbolTable
    : public ISymbolTable<Type>
{
public:
    void Insert(const Symbol& symbol, const Type& entry, bool& overwrite, bool isImmutable = false) override
    {
        overwrite = false;
        if (m_scopeStack.empty())
        {
            throw CompilerErrorException("Attempt to add symbol to symbol table with no scope setup. This is a compiler bug.");
        }

        std::vector<Symbol>& stackFrame = m_scopeStack.top();
        
        if (std::find(begin(stackFrame), end(stackFrame), symbol) != end(stackFrame))
        {
            // Warn about overwriting symbols
            if (LookUp(symbol) != entry)
            {
                overwrite = true;
            }
            else
            {
                throw CompilerErrorException("Re adding symbol to symbol table with same type. This is not a shadowing case");
            }
        }

        m_table[symbol].push({entry, isImmutable});
        stackFrame.push_back(symbol);
    }

    boost::optional<Type> LookUp(const Symbol& symbol, bool* isImmutable = nullptr) const override
    {
        auto iter = m_table.find(symbol);

        boost::optional<Type> elm;
        if (iter != end(m_table) && !iter->second.empty())
        {
            elm = iter->second.top().first;
            if (isImmutable)
            {
                *isImmutable = iter->second.top().second;
            }
        }

        return elm;
    }

    void BeginScope() override
    {
        m_scopeStack.emplace();
    }

    void EndScope() override
    {
        auto stackFrame = m_scopeStack.top();
        m_scopeStack.pop();
        for (const Symbol& sym : stackFrame)
        {
            m_table[sym].pop();
            if (m_table[sym].empty())
            {
                m_table.erase(sym);
            }
        }
    }

    ~SymbolTable() {}
    SymbolTable() {}
private:
    SymbolTable(const SymbolTable&) = delete;

    std::map<Symbol, std::stack<std::pair<Type, bool>>> m_table;
    std::stack<std::vector<Symbol>> m_scopeStack;
    friend struct SymbolTableFactory;
};

struct SymbolTableFactory
{
    template <typename Type>
    static std::shared_ptr<ISymbolTable<Type>> CreateTable()
    {
        return std::make_shared<SymbolTable<Type>>();
    }
};

