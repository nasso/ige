#ifndef AB305E0A_5FE3_4546_A053_FB12E5E64440
#define AB305E0A_5FE3_4546_A053_FB12E5E64440

#include "ige/utility/Platform.hpp"
#include "ige/utility/Types.hpp"
#include <span>

namespace ige::ecs {

/**
 * @brief A Family describes the set of attachments of an Entity.
 *
 * Every Entity belongs to a Family.
 */
class IGE_API Family {
public:
    /**
     * @brief Descriptor for a Family having one attachment more than another.
     */
    struct With {
        const Family& base;
        u64 extra_id;
    };

    /**
     * @brief Descriptor for a Family having one attachment less than another.
     */
    struct Without {
        const Family& base;
        u64 missing_id;

        bool empty() const noexcept;
    };

    /**
     * @brief Helper class for hashing Archetypes.
     */
    struct Hash {
        using is_transparent = void;

        usize operator()(std::span<const u64>) const noexcept;
        usize operator()(const With&) const noexcept;
        usize operator()(const Without&) const noexcept;
        usize operator()(const Family&) const noexcept;
    };

    Family(const Family&) = delete;
    Family& operator=(const Family&) = delete;
    Family(Family&&);
    Family& operator=(Family&&);

    explicit Family(std::span<const u64>);
    explicit Family(const With&);
    explicit Family(const Without&);

    bool operator==(const Family&) const;
    bool operator==(const With&) const;
    bool operator==(const Without&) const;
    bool operator==(std::span<const u64> ids) const;

    auto with(u64 id) const -> With;
    auto without(u64 id) const -> Without;

    bool has(u64 id) const noexcept;

    inline std::span<const u64> ids() const noexcept
    {
        return { m_ids, m_size };
    }

private:
    u64* m_ids;
    usize m_size;
};

}

#endif /* AB305E0A_5FE3_4546_A053_FB12E5E64440 */
