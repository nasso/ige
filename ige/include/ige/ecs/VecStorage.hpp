#ifndef DB322AB1_C834_42D3_82D6_5F31BCC83A33
#define DB322AB1_C834_42D3_82D6_5F31BCC83A33

#include <concepts>
#include <functional>
#include <iterator>
#include <optional>
#include <utility>
#include <vector>

namespace ige::ecs {

template <typename V>
class VecStorage {
private:
    std::vector<std::optional<V>> m_data;

public:
    class Iterator {
    private:
        VecStorage& m_storage;
        std::size_t m_index = 0;

        std::optional<std::pair<std::size_t, V&>> m_value;

        void fetch()
        {
            if (m_index < m_storage.m_data.size()) {
                m_value.emplace(m_index, *m_storage.m_data[m_index]);
            } else {
                m_value.reset();
            }
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::size_t;
        using value_type = std::pair<std::size_t, V&>;
        using pointer = const value_type*;
        using reference = const value_type&;

        Iterator(VecStorage& storage, std::size_t index)
            : m_storage(storage)
            , m_index(index)
        {
            while (m_index < m_storage.m_data.size()
                   && !m_storage.m_data[m_index].has_value()) {
                m_index++;
            }

            fetch();
        }

        reference operator*() const
        {
            return *m_value;
        }

        pointer operator->()
        {
            return &*m_value;
        }

        Iterator& operator++()
        {
            do {
                m_index++;
            } while (m_index < m_storage.m_data.size()
                     && !m_storage.m_data[m_index].has_value());
            fetch();
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator retval = *this;
            ++(*this);
            return retval;
        }

        friend bool operator==(const Iterator& a, const Iterator& b)
        {
            return a.m_index == b.m_index;
        }

        friend bool operator!=(const Iterator& a, const Iterator& b)
        {
            return a.m_index != b.m_index;
        }
    };

    VecStorage() = default;

    VecStorage(VecStorage&& other)
        : m_data(std::move(other.m_data))
    {
    }

    VecStorage& operator=(VecStorage&& rhs)
    {
        m_data = std::move(rhs.m_data);
        return *this;
    }

    template <typename... Args>
        requires std::constructible_from<V, Args...>
    void set(std::size_t idx, Args&&... args)
    {
        if (idx >= m_data.size()) {
            m_data.resize(idx + 1);
        }

        m_data[idx].emplace(std::forward<Args>(args)...);
    }

    const V* get(std::size_t idx) const
    {
        if (idx < m_data.size()) {
            if (auto& value = m_data[idx]) {
                return &*value;
            }
        }

        return nullptr;
    }

    V* get(std::size_t idx)
    {
        if (idx < m_data.size()) {
            if (auto& value = m_data[idx]) {
                return &*value;
            }
        }

        return nullptr;
    }

    std::optional<V> remove(std::size_t idx)
    {
        std::optional<V> element;

        if (idx < m_data.size()) {
            m_data[idx].swap(element);
        }

        return element;
    }

    Iterator begin()
    {
        return { *this, 0 };
    }

    Iterator end()
    {
        return { *this, m_data.size() };
    }
};

}

#endif /* DB322AB1_C834_42D3_82D6_5F31BCC83A33 */
