#pragma once
#include "common.h"
#include "Position.h"
#include <istream>

class PositionCountingStream
{
public:
    PositionCountingStream(std::unique_ptr<std::istream>&& stream);
    unsigned char get();
    unsigned char peek() const;
    unsigned char peekTwo() const;
    bool eof() const;

    Position GetCurrentPosition() const;

private:
    std::unique_ptr<std::istream>&& m_stream;
    uint64_t m_currentRow = 1;
    uint64_t m_currentColumn = 1;
};

