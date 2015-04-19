#include "Frame.h"

#include "common.h"
#include "boost/variant.hpp"

using namespace Temps;
using namespace FrameAccess;
using namespace std;

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
        throw CompilerErrorException("AllocateLocal not yet implemented");
    }

private:
    void CalculateFormals(const vector<bool>& formals)
    {
        if (!m_formals.empty())
        {
            throw CompilerErrorException("Formals should have not been populated");
        }
        int32_t offset = 8; // following c-calling conventions first argument will be at ebp+8
        for (bool formal : formals)
        {
            // all arguments live on the stack and the escape is immaterial
            m_formals.push_back(Access{offset});
            offset += 4;
            // START NONSENSE (to silence warnings)
            formal = !!formal;
            // END NONSENSE
        }
    }

    Label m_label;
    vector<Access> m_formals;
};

std::unique_ptr<Frame> FrameFactory::MakeFrame(const Temps::Label& name, const std::vector<bool>& formals)
{
    return make_unique<X86Frame>(name, formals);
}
