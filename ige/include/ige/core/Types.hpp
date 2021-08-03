#ifndef ECA47615_28F6_4B4E_942D_C8858B5F3899
#define ECA47615_28F6_4B4E_942D_C8858B5F3899

#include <climits>
#include <cstddef>
#include <cstdint>

namespace ige {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using usize = std::uintptr_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using isize = std::intptr_t;
using f32 = float;
using f64 = double;

// things the engine relies on
static_assert(CHAR_BIT == 8, "CHAR_BIT is not 8");
static_assert(sizeof(std::size_t) == sizeof(usize), "size_t != uintptr_t");
static_assert(sizeof(u8) == 1, "sizeof(u8) is not 1");
static_assert(sizeof(u16) == 2, "sizeof(u16) is not 2");
static_assert(sizeof(u32) == 4, "sizeof(u32) is not 4");
static_assert(sizeof(u64) == 8, "sizeof(u64) is not 8");
static_assert(sizeof(i8) == 1, "sizeof(i8) is not 1");
static_assert(sizeof(i16) == 2, "sizeof(i16) is not 2");
static_assert(sizeof(i32) == 4, "sizeof(i32) is not 4");
static_assert(sizeof(i64) == 8, "sizeof(i64) is not 8");
static_assert(sizeof(f32) == 4, "sizeof(f32) is not 4");
static_assert(sizeof(f64) == 8, "sizeof(f64) is not 8");

}

#endif /* ECA47615_28F6_4B4E_942D_C8858B5F3899 */
