#ifndef A33C094A_DF9B_4BBB_B3B2_5AA3DD54CB65
#define A33C094A_DF9B_4BBB_B3B2_5AA3DD54CB65

#include <cstddef>
#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>

namespace detail {

template <typename T>
struct SharedPtrHash {
    std::size_t operator()(const std::shared_ptr<T>& ptr) const
    {
        return std::hash<T*> {}(ptr.get());
    }
};

template <typename T>
struct SharedPtrEq {
    bool operator()(
        const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs) const
    {
        return lhs.get() == rhs.get();
    }
};

}

template <typename K, typename V>
class WeakPtrMap {
    using Map = std::unordered_map<
        std::shared_ptr<K>, V, detail::SharedPtrHash<K>,
        detail::SharedPtrEq<K>>;

public:
    using SharedKey = std::shared_ptr<K>;
    using Iterator = Map::iterator;
    using ConstIterator = Map::const_iterator;

    void clean(bool force = false)
    {
        if (!force) {
            const float lf = static_cast<float>(m_map.size() + 1)
                / static_cast<float>(m_map.bucket_count());

            force = lf >= m_map.max_load_factor();
        }

        if (force) {
            std::erase_if(m_map, [](const auto& item) {
                return item.first.use_count() == 1;
            });
        }
    }

    Iterator end()
    {
        return m_map.end();
    }

    ConstIterator end() const
    {
        return m_map.end();
    }

    Iterator find(const SharedKey& k)
    {
        return m_map.find(k);
    }

    ConstIterator find(const SharedKey& k) const
    {
        return m_map.find(k);
    }

    template <typename... Args>
    std::pair<Iterator, bool> emplace(Args&&... args)
    {
        return m_map.emplace(std::forward<Args>(args)...);
    }

private:
    Map m_map;
};

#endif /* A33C094A_DF9B_4BBB_B3B2_5AA3DD54CB65 */
