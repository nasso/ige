#include "ige/ecs/Table.hpp"
#include "ige/utility/Assert.hpp"
#include "ige/utility/Control.hpp"
#include "igepch.hpp"
#include <cstdlib>

namespace ige::ecs {

Table::Table(std::span<const usize> strides)
    : m_column_count(strides.size())
    , m_column_strides(new usize[m_column_count])
    , m_columns(new void*[m_column_count])
{
    std::copy(strides.begin(), strides.end(), m_column_strides);
    std::fill_n(m_columns, m_column_count, nullptr);
}

Table::~Table()
{
    for (usize i = 0; i < m_column_count; ++i) {
        // todo: call destructors for non-trivially destructible types
        std::free(m_columns[i]);
    }

    delete[] m_columns;
    delete[] m_column_strides;
}

usize Table::insert()
{
    resize(m_row_count + 1);

    return m_row_count - 1;
}

void Table::remove() { resize(m_row_count - 1); }

void Table::remove(usize n)
{
    IGE_ASSERT(n < m_row_count, "row index out of bounds");

    // delete any row in O(1) by swapping it with the last row

    // todo: call destructors for row n

    // swap last row with row n if n is not the last row
    if (n != m_row_count - 1) {
        for (usize i = 0; i < m_column_count; ++i) {
            // todo: use move constructors for non-trivially movable types
            std::copy_n(
                static_cast<const u8*>(cell(i, m_row_count - 1)),
                m_column_strides[i],
                static_cast<u8*>(cell_mut(i, n)));
        }
    }

    // decrement row count
    m_row_count--;
}

void Table::remove_column(usize n)
{
    IGE_ASSERT(n < m_column_count, "column index out of bounds");

    // remove column n
    // todo: call destructors for column n
    std::free(m_columns[n]);

    // decrement column count
    m_column_count--;

    // shift all columns after n to the left
    for (usize i = n; i < m_column_count; ++i) {
        m_column_strides[i] = m_column_strides[i + 1];
        m_columns[i] = m_columns[i + 1];
    }

    // invalidate the last column since it was moved
    m_column_strides[m_column_count] = 0;
    m_columns[m_column_count] = nullptr;
}

void Table::resize(usize n)
{
    if (n < m_row_count) {
        // todo: call destructors for rows n to m_row_count

        // remove rows
        m_row_count = n;
    } else if (n > m_row_count) {
        // grow each column to make space for the new rows
        // perf: allocate more space than necessary to avoid frequent
        // reallocations
        for (usize i = 0; i < m_column_count; ++i) {
            const usize stride = m_column_strides[i];
            void* column = m_columns[i];
            void* new_column = std::malloc(stride * n);

            IGE_ASSERT(new_column != nullptr, "allocation failed");

            // copy old data to new column
            // todo: use move constructors for non-trivially movable types
            std::copy_n(
                static_cast<u8*>(column),
                stride * m_row_count,
                static_cast<u8*>(new_column));

            // delete old column
            std::free(column);

            // set new column
            m_columns[i] = new_column;
        }

        m_row_count = n;
    }
}

}
