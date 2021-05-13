#ifndef DB322AB1_C834_42D3_82D6_5F31BCC83A33
#define DB322AB1_C834_42D3_82D6_5F31BCC83A33

#include <concepts>
#include <functional>
#include <iterator>
#include <optional>
#include <utility>
#include <vector>

namespace ige {
namespace ecs {

    template <typename T>
    class VecStorage {
    private:
        std::vector<std::optional<T>> m_data;

    public:
        class Iterator {
        private:
            VecStorage& m_storage;
            std::size_t m_index = 0;

            std::optional<std::pair<std::size_t, T&>> m_value;

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
            using value_type = std::pair<std::size_t, T&>;
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
            requires std::constructible_from<T, Args...>
        void set(std::size_t idx, Args&&... args)
        {
            m_data.resize(idx + 1);
            m_data.emplace(m_data.begin() + idx,
                std::make_optional<T>(std::forward<Args>(args)...));
        }

        std::optional<std::reference_wrapper<const T>> get(
            const std::size_t& idx) const
        {
            std::optional<std::reference_wrapper<const T>> element;

            if (idx < m_data.size()) {
                if (auto& value = m_data[idx]) {
                    element.emplace(*value);
                }
            }

            return element;
        }

        std::optional<std::reference_wrapper<T>> get(const std::size_t& idx)
        {
            std::optional<std::reference_wrapper<T>> element;

            if (idx < m_data.size()) {
                if (auto& value = m_data[idx]) {
                    element.emplace(*value);
                }
            }

            return element;
        }

        std::optional<T> remove(const std::size_t& idx)
        {
            std::optional<T> element;

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
}

#endif /* DB322AB1_C834_42D3_82D6_5F31BCC83A33 */
