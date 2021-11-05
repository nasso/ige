#include "ige/utility/FamilyGen.hpp"
#include "ige/utility/Types.hpp"

namespace ige::utility {

u64 FamilyGen::next() noexcept
{
    static u64 counter = 0;

    return counter++;
}

}
