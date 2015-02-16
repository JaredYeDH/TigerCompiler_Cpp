#include "Position.h"

Position::Position(uint64_t row, uint64_t column)
    : m_row(row)
    , m_column(column)
{}
    
Position::Position(const Position& other)
    : m_row(other.GetRow())
    , m_column(other.GetColumn())
{}

uint64_t Position::GetRow() const
{
    return m_row;
}

uint64_t Position::GetColumn() const
{
    return m_column;
}

bool Position::operator==(const Position& other) const
{
    return GetRow() == other.GetRow() && GetColumn() == other.GetColumn();
}

bool Position::operator!=(const Position& other) const
{
    return GetRow() != other.GetRow() || GetColumn() != other.GetColumn();
}

bool Position::operator<(const Position& other) const
{
    return GetRow() < other.GetRow() || (GetRow() == other.GetRow() && GetColumn() < other.GetColumn());
}

