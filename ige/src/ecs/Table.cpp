#include "ige/ecs/Table.hpp"
#include "ige/utility/Assert.hpp"
#include "ige/utility/Control.hpp"

namespace ige::ecs {

Table::Table(std::span<const ComponentMeta> metas)
    : m_component_count(metas.size())
{
    m_components = new ComponentArray[m_component_count];
}

Table::~Table() { delete[] m_components; }

usize Table::insert(const void** components)
{
    for (usize comp = 0; comp < m_component_count; ++comp) {
        const void* data = components[comp];
        ComponentArray& array = m_components[comp];

        array.data.resize(array.data.size() + array.meta.size);
        std::copy(
            static_cast<const u8*>(data),
            static_cast<const u8*>(data) + array.meta.size,
            array.data.end() - array.meta.size);
    }

    return m_entries++;
}

void Table::remove(usize) { IGE_TODO(); }

void* Table::get_mut(usize) { return IGE_TODO(void*); }

const void* Table::get(usize) const { return IGE_TODO(const void*); }

}
