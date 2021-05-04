/*
** EPITECH PROJECT, 2021
** ige
** File description:
** VecStorage
*/

#ifndef DB322AB1_C834_42D3_82D6_5F31BCC83A33
#define DB322AB1_C834_42D3_82D6_5F31BCC83A33

#include <concepts>
#include <functional>
#include <optional>
#include <vector>

namespace ige {
namespace ecs {

    template <typename T>
    class VecStorage {
    public:
        VecStorage() = default;
        virtual ~VecStorage() = default;

        VecStorage(VecStorage&& other)
            : m_data(std::move(other.m_data))
        {
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

    private:
        std::vector<std::optional<T>> m_data;
    };

}
}

#endif /* DB322AB1_C834_42D3_82D6_5F31BCC83A33 */
