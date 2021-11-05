#include "FamilyGen.hpp"

namespace ige::utility {

template <class T>
u64 FamilyGen::type() noexcept
{
    static const u64 id = next();

    return id;
}

}
