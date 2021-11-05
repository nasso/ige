#ifndef D17A52BF_A19B_4751_A063_0360ACB00065
#define D17A52BF_A19B_4751_A063_0360ACB00065

#include "ige/utility/Platform.hpp"
#include "ige/utility/Types.hpp"

namespace ige::utility {

class IGE_API FamilyGen {
public:
    template <class T>
    static u64 type() noexcept;

private:
    static u64 next() noexcept;
};

}

#include "FamilyGen.inl"

#endif /* D17A52BF_A19B_4751_A063_0360ACB00065 */
