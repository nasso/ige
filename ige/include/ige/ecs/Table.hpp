#ifndef DBF47B0D_537F_4A39_80AC_DE15582D2627
#define DBF47B0D_537F_4A39_80AC_DE15582D2627

#include "ige/ecs/Component.hpp"
#include "ige/utility/Assert.hpp"
#include "ige/utility/Platform.hpp"
#include "ige/utility/Types.hpp"
#include <span>

namespace ige::ecs {

/**
 * @brief A Table stores arbitrary bytes of data in "SoA" style.
 *
 * Once created, a Table cannot be extended horizontally. However, new rows can
 * be dynamically added or removed. Each column is stored as a contiguous memory
 * block and cannot contain "holes".
 *
 * A Table looks like the following diagram:
 *
 * ```
 * +-------------------------------------------------------+
 * | Component 1 | Component 2 | Component 3 | Component 4 |
 * | Component 1 | Component 2 | Component 3 | Component 4 |
 * | Component 1 | Component 2 | Component 3 | Component 4 |
 * | Component 1 | Component 2 | Component 3 | Component 4 |
 * +-------------------------------------------------------+
 * ```
 *
 * Which can be described by the following structure:
 *
 * ```
 * struct ExampleTable {
 *   Component1* row1;
 *   Component2* row2;
 *   Component3* row3;
 *   Component4* row4;
 * }
 * ```
 */
class IGE_API Table {
public:
    /**
     * @brief Create an empty table storage.
     *
     * @param strides The size of one element in each column.
     */
    Table(std::span<const usize> strides);

    /**
     * @brief Table destructor.
     */
    ~Table();

    /**
     * @brief Add a new row to the table.
     *
     * @return The index of the new row.
     */
    usize insert();

    /**
     * @brief Remove the last row from the table.
     */
    void remove();

    /**
     * @brief Remove the specified row from the table.
     *
     * @param row The index of the row to remove.
     */
    void remove(usize row);

    /**
     * @brief Resize the Table so that it contains exactly n rows.
     */
    void resize(usize n);

    /**
     * @brief Get a pointer to the first element of the specified column.
     *
     * @param column The column to get the pointer to.
     */
    inline void* col_mut(usize n)
    {
        IGE_ASSERT(n < m_column_count, "column out of bounds.");

        return m_columns[n];
    }

    /**
     * @brief Get a pointer to the first element of the specified column.
     *
     * @param column The column to get the pointer to.
     */
    inline const void* column(usize n) const
    {
        IGE_ASSERT(n < m_column_count, "column out of bounds.");

        return m_columns[n];
    }

    /**
     * @brief Get the number of rows in the table.
     */
    inline usize row_count() const noexcept { return m_row_count; }

    /**
     * @brief Get the sizes of each column.
     */
    inline std::span<const usize> strides() const noexcept
    {
        return { m_column_strides, m_column_count };
    }

private:
    const usize m_column_count;
    usize* m_column_strides;
    void** m_columns;
    usize m_row_count = 0;
};

}

#endif /* DBF47B0D_537F_4A39_80AC_DE15582D2627 */
