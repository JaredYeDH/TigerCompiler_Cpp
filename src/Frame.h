#pragma once
#include "Temps.h"

class Access;

class Frame
{
public:
    virtual Temps::Label GetName() = 0;
    virtual const std::vector<Access>& UseFormals() = 0;
    virtual Access AllocateLocal(bool escapes) = 0;
};

namespace FrameFactory
{
    std::unique_ptr<Frame> MakeFrame(const Temps::Label& name, const std::vector<bool>& formals);
}
