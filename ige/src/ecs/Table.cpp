#include "ige/ecs/Table.hpp"
#include "ige/utility/Assert.hpp"
#include "ige/utility/Control.hpp"
#include "igepch.hpp"

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
        delete[] m_columns[i];
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

    // decrement row count
    m_row_count--;

    // swap last row with row n if n is not the last row
    if (n != m_row_count) {
        for (usize i = 0; i < m_column_count; ++i) {
            const usize stride = m_column_strides[i];
            u8* column = static_cast<u8*>(m_columns[i]);

            // todo: use move constructors for non-trivially movable types
            std::copy_n(
                column + stride * m_row_count,
                stride,
                column + stride * n);
        }
    }
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
            u8* column = static_cast<u8*>(m_columns[i]);
            u8* new_column = new u8[stride * n];

            // copy old data to new column
            // todo: use move constructors for non-trivially movable types
            std::copy_n(column, stride * m_row_count, new_column);

            // delete old column
            delete[] column;

            // set new column
            m_columns[i] = static_cast<void*>(new_column);
        }

        m_row_count = n;
    }
}

}
