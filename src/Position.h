#pragma once
#include "common.h"

class Position
{
public:
    Position(uint64_t row, uint64_t column);
    Position(const Position& other);
    uint64_t GetRow() const;
    uint64_t GetColumn() const;
    
    bool operator==(const Position& other) const;
    bool operator!=(const Position& other) const;
    bool operator<(const Position& other) const;
    
private:
    uint64_t m_row;
    uint64_t m_column;
};
