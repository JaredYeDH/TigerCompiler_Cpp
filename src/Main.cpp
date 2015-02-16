#include "common.h"
#include "PositionCountingStream.h"
#include <sstream>

int main()
{
    std::unique_ptr<std::istream> inStream = make_unique<std::stringstream>("a program");
    auto stream = PositionCountingStream(std::move(inStream));
    stream.get();
    stream.GetCurrentPosition();
}
