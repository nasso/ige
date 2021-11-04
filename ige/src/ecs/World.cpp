#include "ige/ecs/World.hpp"
#include "ige/ecs/System.hpp"
#include "ige/utility/Assert.hpp"
#include "ige/utility/Control.hpp"
#include "igepch.hpp"

namespace ige::ecs {

static const TypeInfo TYPE_INFO_INFO { sizeof(TypeInfo), alignof(TypeInfo) };

World::World()
    : m_free_entities(new EntityList)
    , m_entity_index(new EntityIndex)
    , m_archetypes(new ArchetypeIndex)
{
    add(m_comp_type_info, m_comp_type_info);
}

World::~World()
{
    delete m_free_entities;
    delete m_entity_index;
    delete m_archetypes;
}

Entity World::entity() { return entity({ empty_archetype(), 0 }); }

void World::destroy(Entity entity)
{
    // remove from all entities that had this entity as a component
    auto it = m_archetypes->begin();

    while (it != m_archetypes->end()) {
        ArchetypeRecord ar(it->first, it->second);
        const Family& fam = ar.family();

        // this family of entity has this entity as a component
        if (fam.has(entity.id())) {
            // lookup the family that does not have it
            auto tit = m_archetypes->find(fam.without(entity.id()));

            if (tit != m_archetypes->end()) {
                ArchetypeRecord tar(tit->first, tit->second);

                // we found a family that is the same as fam without entity
                // just move all entities there
                migrate_all(tar, ar);
                m_archetypes->erase(it++);
            } else {
                // it doesn't exist, so we have to create it
                // since fam won't exist anymore, we can safely replace it
                auto nh = m_archetypes->extract(it++);

                // fixme: mutate the family instead of creating a new one
                nh.key() = Family(fam.without(entity.id()));
                m_archetypes->insert(std::move(nh));
            }
        } else {
            ++it;
        }
    }

    m_entity_index->erase(entity.idgen());
    m_free_entities->emplace_back(entity.id(), entity.gen() + 1);
}

bool World::is_alive(Entity entity) const
{
    return get_record(entity) != nullptr;
}

void World::add(Entity entity, u64 id)
{
    IGE_ASSERT(is_alive(entity), "\"entity\" is not alive");

    Record& record = *get_record_mut(entity);

    add_to_record(record, id);
}

void World::remove(Entity entity, u64 id)
{
    IGE_ASSERT(is_alive(entity), "\"entity\" is not alive");

    Record& record = *get_record_mut(entity);

    remove_from_record(record, id);
}

bool World::has(Entity entity, u64 id) const
{
    IGE_ASSERT(is_alive(entity), "\"entity\" is not alive");

    return get_record(entity)->family().has(id);
}

Entity World::component(usize size, usize alignment)
{
    const TypeInfo info { size, alignment };

    Entity e = entity();

    set(e, m_comp_type_info, &info);

    return e;
}

void World::set(Entity entity, Entity component, const void* data)
{
    // get the TypeInfo for the component
    const TypeInfo* type_info = get_component_type_info(component);

    // if ptr is null, then "component" isn't actually a component
    IGE_ASSERT(type_info != nullptr, "\"component\" isn't a component");

    mutate(entity, component, [&](void* ptr) {
        u8* dest = static_cast<u8*>(ptr);
        const u8* src = static_cast<const u8*>(data);

        std::copy_n(src, type_info->size, dest);
    });
}

const void* World::get(Entity entity, Entity component) const
{
    // hardcode the value of TypeInfo for m_comp_type_info
    if (entity == m_comp_type_info && component == m_comp_type_info) {
        return static_cast<const void*>(&TYPE_INFO_INFO);
    }

    const Record* record = get_record(entity);

    IGE_ASSERT(record != nullptr, "\"entity\" is not alive");

    const Table* table = record->archetype().table;

    if (!table || table->row_count() <= record->row()) {
        // the entity doesn't have the component
        return nullptr;
    }

    const usize column = get_component_column(record->family(), component);

    return table->cell(column, record->row());
}

Query<> World::query() { return Query(*this); }

SystemBuilder<> World::system() { return SystemBuilder(*this); }

void World::update()
{
    m_tick++;
    IGE_TODO();
}

bool World::is_component(u64 id) const
{
    u32 lid = id & 0xFFFFFFFF;

    if (lid != id) {
        return false;
    }

    return has(Entity(lid), m_comp_type_info);
}

/******************************************************************************/
/*                                                                            */
/*                              PRIVATE  METHODS                              */
/*                                                                            */
/******************************************************************************/

Entity World::entity(Record record)
{
    const bool recycle = !m_free_entities->empty();
    Entity e = recycle ? m_free_entities->back() : Entity(m_last_entity_id, 0);

    if (recycle) {
        m_free_entities->pop_back();
    } else {
        m_last_entity_id++;
    }

    set_record(e, record);

    return e;
}

Table* World::touch_table(ArchetypeRecord& ar)
{
    if (!ar.archetype().table) {
        std::vector<usize> sizes;

        for (u64 id : ar.family().ids()) {
            u32 lid = id & 0xFFFFFFFF;

            if (id == lid) {
                const TypeInfo* info = get_component_type_info(Entity(lid));

                if (info) {
                    sizes.push_back(info->size);
                }
            }
        }

        // if the family doesn't contain any components, then the table is empty
        // so we don't need to create it
        if (!sizes.empty()) {
            ar.archetype_mut().table = new Table(sizes);
        }
    }

    return ar.archetype().table;
}

const TypeInfo* World::get_component_type_info(Entity component) const
{
    return static_cast<const TypeInfo*>(get(component, m_comp_type_info));
}

void World::set_record(Entity e, Record record)
{
    m_entity_index->emplace(e.idgen(), std::move(record));
}

const Record* World::get_record(Entity entity) const
{
    auto it = m_entity_index->find(entity.idgen());

    if (it != m_entity_index->end()) {
        return &it->second;
    }

    return nullptr;
}

Record* World::get_record_mut(Entity entity)
{
    auto it = m_entity_index->find(entity.idgen());

    if (it != m_entity_index->end()) {
        return &it->second;
    }

    return nullptr;
}

void World::add_to_record(Record& record, u64 id)
{
    auto ar = get_archetype(record.family().with(id));

    migrate_record(ar, record);
}

void World::remove_from_record(Record& record, u64 id)
{
    auto ar = get_archetype(record.family().without(id));

    migrate_record(ar, record);
}

usize World::get_component_column(const Family& fam, Entity component) const
{
    usize column = 0;

    for (u64 id : fam.ids()) {
        if (id == component.id()) {
            IGE_ASSERT(is_component(id), "\"component\" isn't a component");
            return column;
        } else if (is_component(id)) {
            column++;
        }
    }

    return column;
}

void World::migrate_record(ArchetypeRecord& dst_ar, Record& record)
{
    const Family& src_fam = record.family();
    const Family& dst_fam = dst_ar.family();
    Table*& src_table = record.archetype_mut().table;
    Table*& dst_table = dst_ar.archetype_mut().table;

    if (src_table && src_table == dst_table) {
        // the old and new table are the same, nothing to be done

        // just update the record's type
        record.type_mut() = dst_ar;
        return;
    }

    // create the destination table if it doesn't exist
    touch_table(dst_ar);

    // if there is both a source and a destination table, move the data
    if (dst_table && src_table) {
        const usize src_row = record.row();
        usize dst_row = dst_table->insert();
        usize dst_col = 0;

        for (u64 id : dst_fam.ids()) {
            u32 lid = id & 0xFFFFFFFF;

            if (id == lid && src_fam.has(id) && is_component(id)) {
                usize src_col = get_component_column(src_fam, Entity(lid));

                std::copy_n(
                    static_cast<const u8*>(src_table->cell(src_col, src_row)),
                    src_table->strides()[src_col],
                    static_cast<u8*>(dst_table->cell_mut(dst_col, dst_row)));
            }

            dst_col++;
        }

        src_table->remove(record.row());

        // update the row for all records after the one that was removed
        // perf: this is O(n) over the number of entities in the world
        for (auto& [id, r] : *m_entity_index) {
            if (r.archetype().table == src_table && r.row() > record.row()) {
                r.row_mut()--;
            }
        }

        // of course, we also need to update the row of the record we're moving
        record.row_mut() = dst_row;
    }

    // finally, update the record's type
    record.type_mut() = dst_ar;
}

void World::migrate_all(ArchetypeRecord& dst, const ArchetypeRecord& src)
{
    // const Family& dst_fam = dst.family();
    // const Family& src_fam = src.family();

    Table* dst_table = dst.archetype_mut().table;
    const Table* src_table = src.archetype().table;

    // update all records that use the source table
    // perf: this is O(n) over the number of entities in the world
    for (auto& [id, r] : *m_entity_index) {
        if (r.archetype().table == src_table) {
            r.type_mut() = dst;
        }
    }

    if (src_table == dst_table) {
        // the old and new table are the same, nothing to be done
        return;
    }

    IGE_TODO();
}

ArchetypeRecord World::empty_archetype()
{
    return get_archetype(std::span<const u64>());
}

ArchetypeRecord World::get_archetype(std::span<const u64> ids)
{
    auto it = m_archetypes->find(ids);

    if (it == m_archetypes->end()) {
        it = m_archetypes->emplace(ids, Archetype()).first;
    }

    return { it->first, it->second };
}

ArchetypeRecord World::get_archetype(Family::With w)
{
    auto it = m_archetypes->find(w);

    if (it == m_archetypes->end()) {
        it = m_archetypes->emplace(w, Archetype()).first;
    }

    return { it->first, it->second };
}

ArchetypeRecord World::get_archetype(Family::Without w)
{
    auto it = m_archetypes->find(w);

    if (it == m_archetypes->end()) {
        it = m_archetypes->emplace(w, Archetype()).first;
    }

    return { it->first, it->second };
}

}
