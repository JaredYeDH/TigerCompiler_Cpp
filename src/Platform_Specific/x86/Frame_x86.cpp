#include "Frame.h"

#include "common.h"
#include "boost/variant.hpp"

using namespace Temps;
using namespace FrameAccess;
using namespace std;

enum class X86Register : uint8_t
{
    EAX,
    EBX,
    ECX,
    EDX,
    EDI,
    ESI
};

class X86Frame
    : public Frame
{
public:
    X86Frame(const Label& name, const vector<bool>& formals)
        : m_label(name)
    {
        CalculateFormals(formals);
    }

    Temps::Label GetName() const override
    {
        return m_label;
    }

    const std::vector<Access>& UseFormals() const override
    {
        return m_formals;
    }

    Access AllocateLocal(bool escapes) override
    {
        if (!escapes)
        {
            return TryPlaceInRegister();
        }
        else
        {
            return AllocateLocalOnStack();
        }
    }

private:
    void CalculateFormals(const vector<bool>& formals)
    {
        if (!m_formals.empty())
        {
            throw CompilerErrorException("Formals should have not been populated");
        }
        for (bool formal : formals)
        {
            // all arguments live on the stack and the escape is immaterial
            m_formals.push_back(AllocateLocalOnStack());
            
            // START NONSENSE (to silence warnings)
            formal = !!formal;
            // END NONSENSE
        }
    }

    Access AllocateLocalOnStack()
    {
        Access access{m_currentOffset};
        m_currentOffset += 4;
        return access;
    }

    Access TryPlaceInRegister()
    {
        if (m_freeRegisters.empty())
        {
            return AllocateLocalOnStack();
        }
        m_freeRegisters.pop_back();
        return Access{UseTempFactory().MakeTempVar()};
    }

    Label m_label;
    vector<Access> m_formals;
    vector<X86Register> m_freeRegisters {
        X86Register::EAX,
        X86Register::EBX,
        X86Register::ECX,
        X86Register::EDX,
        X86Register::EDI,
        X86Register::ESI
    };
    int32_t m_currentOffset = 8;
};

std::unique_ptr<Frame> FrameFactory::MakeFrame(const Temps::Label& name, const std::vector<bool>& formals)
{
    return make_unique<X86Frame>(name, formals);
}
