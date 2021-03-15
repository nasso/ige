/*
** EPITECH PROJECT, 2021
** un
** File description:
** MapStorage
*/

#ifndef F171BC61_6D7C_4555_A4C6_5073CCB074F3
#define F171BC61_6D7C_4555_A4C6_5073CCB074F3

#include "IStorage.hpp"
#include "rtl/Option.hpp"
#include <unordered_map>

namespace un {
namespace ecs {

    template <typename K, typename V>
    class MapStorage : public IStorage<K, V> {
    public:
        MapStorage() = default;
        virtual ~MapStorage() = default;

        virtual void set(K&& key, V&& value) override
        {
            remove(key);
            m_data.emplace(std::forward<K>(key), std::forward<V>(value));
        }

        virtual rtl::Option<const V&> get(const K& key) const override
        {
            auto& it = m_data.find(key);

            if (it != m_data.end()) {
                return rtl::some(it->second);
            } else {
                return {};
            }
        }

        virtual rtl::Option<V&> get(const K& key) override
        {
            auto& it = m_data.find(key);

            if (it != m_data.end()) {
                return rtl::some(it->second);
            } else {
                return {};
            }
        }

        virtual rtl::Option<V> remove(const K& key) override
        {
            auto& it = m_data.find(key);

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
