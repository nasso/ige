#ifndef DBF47B0D_537F_4A39_80AC_DE15582D2627
#define DBF47B0D_537F_4A39_80AC_DE15582D2627

#include "ige/ecs/Component.hpp"
#include "ige/utility/Platform.hpp"
#include "ige/utility/Types.hpp"
#include <span>
#include <unordered_map>
#include <vector>

namespace ige::ecs {

/**
 * @brief A ComponentMeta stores information about a Component data type.
 */
struct ComponentMeta {
    template <Component C>
    inline static ComponentMeta get() noexcept
    {
        return { sizeof(C), alignof(C) };
    }

    usize size = 0;
    usize alignment = 0;
};

/**
 * @brief A Table stores component data for an archetype.
 *
 * Data is stored in SoA format, i.e. the data for each component is stored
 * contiguously in memory, as in the following pseudo-structure:
 *
 * ```
 * struct ExampleTable {
 *   FooComponent foos[n];
 *   BarComponent bars[n];
 *   XyzComponent xyzs[n];
 *   ...
 * };
 * ```
 */
class IGE_API Table {
public:
    /**
     * @brief Create an empty table storage for the given components.
     *
     * @param metas Type information for each component data-type.
     */
    Table(std::span<const ComponentMeta> metas);

    ~Table();

    /**
     * @brief Add a new entry to the table.
     *
     * @param components The data of each component for the new entry.
     * @return The index of the new entry.
     */
    usize insert(const void** components);

    /**
     * @brief Remove an entry from the table.
     *
     * @param index The index of the entry to remove.
     */
    void remove(usize index);

    /**
     * @brief Get the component data for an entry.
     *
     * @param index The index of the entry.
     * @return The component data for the entry.
     */
    void* get_mut(usize index);

    /**
     * @brief Get the component data for an entry.
     *
     * @param index The index of the entry.
     * @return The component data for the entry.
     */
    const void* get(usize index) const;

    /**
     * @brief Get the number of entries in the table.
     *
     * @return The number of entries in the table.
     */
    inline usize size() const noexcept { return m_component_count; }

private:
    struct ComponentArray {
        ComponentMeta meta = {};
        std::vector<u8> data = {};
    };

    ComponentArray* m_components = nullptr;
    usize m_component_count = 0;
    usize m_entries = 0;
};

}

#endif /* DBF47B0D_537F_4A39_80AC_DE15582D2627 */
