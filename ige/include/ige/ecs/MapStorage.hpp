/*
** EPITECH PROJECT, 2021
** ige
** File description:
** MapStorage
*/

#ifndef F171BC61_6D7C_4555_A4C6_5073CCB074F3
#define F171BC61_6D7C_4555_A4C6_5073CCB074F3

#include <concepts>
#include <functional>
#include <optional>
#include <unordered_map>

namespace ige {
namespace ecs {

    template <typename K, typename V>
    class MapStorage {
    public:
        MapStorage() = default;
        virtual ~MapStorage() = default;

        MapStorage(MapStorage&& other)
            : m_data(std::move(other.m_data))
        {
        }

        template <typename... Args>
            requires std::constructible_from<V, Args...>
        void set(K&& key, Args&&... args)
        {
            remove(key);
            m_data.emplace(std::piecewise_construct, std::forward_as_tuple(key),
                std::forward_as_tuple(std::forward<Args>(args)...));
        }

        std::optional<std::reference_wrapper<const V>> get(const K& key) const
        {
            auto it = m_data.find(key);

            if (it != m_data.end()) {
                return { it->second };
            } else {
                return {};
            }
        }

        std::optional<std::reference_wrapper<V>> get(const K& key)
        {
            auto it = m_data.find(key);

            if (it != m_data.end()) {
                return { it->second };
            } else {
                return {};
            }
        }

        std::optional<V> remove(const K& key)
        {
            auto it = m_data.find(key);

            if (it != m_data.end()) {
                V opt = std::move(it->second);

                m_data.erase(it);
                return { std::move(opt) };
            } else {
                return {};
            }
        }

    private:
        std::unordered_map<K, V> m_data;
    };

}
}

#endif /* F171BC61_6D7C_4555_A4C6_5073CCB074F3 */
