#include "ige/ecs/Family.hpp"
#include "igepch.hpp"

namespace ige::ecs {

bool Family::Without::empty() const noexcept
{
    const auto ids = base.ids();

    return ids.empty() || (ids.size() == 1 && ids.front() == missing_id);
}

usize Family::Hash::operator()(std::span<const u64> ids) const noexcept
{
    usize hash = 17;

    for (auto id : ids) {
        hash = hash * 31 + std::hash<u64>()(id);
    }

    return hash;
}

usize Family::Hash::operator()(const With& w) const noexcept
{
    usize hash = 17;

    for (auto id : w.base.ids()) {
        if (id > w.extra_id) {
            hash = hash * 31 + std::hash<u64>()(w.extra_id);
        }

        hash = hash * 31 + std::hash<u64>()(id);
    }

    return hash;
}

usize Family::Hash::operator()(const Without& w) const noexcept
{
    usize hash = 17;

    for (auto id : w.base.ids()) {
        if (id != w.missing_id) {
            hash = hash * 31 + std::hash<u64>()(id);
        }
    }

    return hash;
}

usize Family::Hash::operator()(const Family& a) const noexcept
{
    return operator()(a.ids());
}

Family::Family(std::span<const u64> ids)
{
    m_ids = new u64[ids.size()];
    m_size = ids.size();

    std::copy(ids.begin(), ids.end(), m_ids);
    std::sort(m_ids, m_ids + m_size);
}

Family::Family(With w)
{
    auto ids = w.base.ids();

    if (w.base.has(w.extra_id)) {
        m_ids = new u64[ids.size()];
        m_size = ids.size();

        std::copy(ids.begin(), ids.end(), m_ids);
        // no need to sort, because ids in w.base are already sorted
    } else {
        m_ids = new u64[ids.size() + 1];
        m_size = ids.size() + 1;

        std::merge(ids.begin(), ids.end(), &w.extra_id, &w.extra_id + 1, m_ids);
    }
}

Family::Family(Without w)
{
    auto ids = w.base.ids();

    if (w.base.has(w.missing_id)) {
        m_ids = new u64[ids.size() - 1];
        m_size = ids.size() - 1;

        std::set_difference(
            ids.begin(),
            ids.end(),
            &w.missing_id,
            &w.missing_id + 1,
            m_ids);
    } else {
        m_ids = new u64[ids.size()];
        m_size = ids.size();

        std::copy(ids.begin(), ids.end(), m_ids);
    }
}

Family::Family(Family&& a)
    : m_ids(a.m_ids)
    , m_size(a.m_size)
{
    a.m_ids = nullptr;
    a.m_size = 0;
}

Family& Family::operator=(Family&& f)
{
    if (this != &f) {
        delete[] m_ids;

        m_ids = f.m_ids;
        m_size = f.m_size;

        f.m_ids = nullptr;
        f.m_size = 0;
    }

    return *this;
}

bool Family::operator==(std::span<const u64> rhs) const
{
    // ids are sorted at construction, so we can use std::equal
    return std::equal(ids().begin(), ids().end(), rhs.begin(), rhs.end());
}

bool Family::operator==(const Family& rhs) const
{
    return std::equal(
        ids().begin(),
        ids().end(),
        rhs.ids().begin(),
        rhs.ids().end());
}

bool Family::operator==(const With& w) const
{
    auto w_ids = w.base.ids();

    // if m_ids is empty or smaller than w_ids, no match is possible
    if (m_size == 0 || m_size < w_ids.size()) {
        return false;
    }

    // handle the case where w_ids is empty
    // m_ids must be [w.extra_id]
    if (w_ids.empty()) {
        return m_size == 1 && m_ids[0] == w.extra_id;
    }

    // now we know m_ids and w_ids are both sorted and non-empty

    // compare m_ids to desc_ids where:
    // l = all x in w_ids where x < w.extra_id
    // g = all x in w_ids where x > w.extra_id
    // desc_ids = [l... w.extra_id g...]

    for (usize i = 0; i < m_size; i++) {
        u64 id = m_ids[i];

        if (id == w.extra_id) {
            // we found the extra id, so we can compare the rest of the ids
            // (the g part of the ids)
            return std::equal(
                w_ids.begin() + i,
                w_ids.end(),
                ids().begin() + i);
        } else if (id > w.extra_id || id != w_ids[i]) {
            // we never found the extra id
            // id < w.extra_id, and we found a mismatch
            break;
        }
    }

    return false;
}

bool Family::operator==(const Without& w) const
{
    auto w_ids = w.base.ids();

    auto x = std::lower_bound(w_ids.begin(), w_ids.end(), w.missing_id);

    if (x == w_ids.end()) {
        // w.missing_id is not in w_ids
        return std::equal(
            ids().begin(),
            ids().end(),
            w_ids.begin(),
            w_ids.end());
    }

    return std::equal(w_ids.begin(), x, ids().begin())
        && std::equal(
               x + 1,
               w_ids.end(),
               ids().begin() + (w_ids.end() - x),
               ids().end());
}

auto Family::with(u64 id) const -> With { return { *this, id }; }

auto Family::without(u64 id) const -> Without { return { *this, id }; }

bool Family::has(u64 id) const noexcept
{
    // we can use std::binary_search because ids are sorted
    return std::binary_search(ids().begin(), ids().end(), id);
}

}
