#include "PositionCountingStream.h"

using namespace std;

PositionCountingStream::PositionCountingStream(unique_ptr<istream>&& stream)
    : m_stream(move(stream))
{}

unsigned char PositionCountingStream::get()
{
    unsigned char ch = m_stream->get();
    if (ch == '\n')
    {
        m_currentRow++;
        m_currentColumn = 1;
    }
    else
    {
        m_currentColumn++;
    }
    return ch;
}

unsigned char PositionCountingStream::peek() const
{
    return m_stream->peek();
}

unsigned char PositionCountingStream::peekTwo() const
{
    m_stream->get();
    auto c = m_stream->peek();
    m_stream->unget();
    return c;
}

bool PositionCountingStream::eof() const
{
    return m_stream->eof();
}

Position PositionCountingStream::GetCurrentPosition() const
{
    return Position(m_currentRow, m_currentColumn);
}
