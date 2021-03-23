/*
** EPITECH PROJECT, 2021
** un
** File description:
** MapStorage
*/

#ifndef F171BC61_6D7C_4555_A4C6_5073CCB074F3
#define F171BC61_6D7C_4555_A4C6_5073CCB074F3

#include "rtl/Option.hpp"
#include <unordered_map>

namespace un {
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

        template <typename... Args,
            typename = std::enable_if_t<
                std::is_constructible<V, Args...>::value>>
        void set(K&& key, Args&&... args)
        {
            remove(key);
            m_data.emplace(std::piecewise_construct,
                std::forward_as_tuple(key),
                std::forward_as_tuple(std::forward<Args>(args)...));
        }

        rtl::Option<const V&> get(const K& key) const
        {
            auto it = m_data.find(key);

            if (it != m_data.end()) {
                return rtl::some(it->second);
            } else {
                return {};
            }
        }

        rtl::Option<V&> get(const K& key)
        {
            auto it = m_data.find(key);

            if (it != m_data.end()) {
                return rtl::some(it->second);
            } else {
                return {};
            }
        }

        rtl::Option<V> remove(const K& key)
        {
            auto it = m_data.find(key);

            if (it != m_data.end()) {
                V opt = std::move(it->second);

                m_data.erase(it);
                return rtl::some(std::move(opt));
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
