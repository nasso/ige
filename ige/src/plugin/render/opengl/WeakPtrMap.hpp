#ifndef A33C094A_DF9B_4BBB_B3B2_5AA3DD54CB65
#define A33C094A_DF9B_4BBB_B3B2_5AA3DD54CB65

#include <cstddef>
#include <functional>
#include <memory>
#include <unordered_map>

namespace detail {

template <typename T>
struct WeakPtrHash {
    std::size_t operator()(const std::weak_ptr<T>& ptr) const
    {
        std::size_t res = 17;
        res = res * 31 + std::hash<bool>()(ptr.expired());
        res = res * 31 + std::hash<T*>()(ptr.lock().get());
        return res;
    }
};

template <typename T>
struct WeakPtrEq {
    bool
    operator()(const std::weak_ptr<T>& lhs, const std::weak_ptr<T>& rhs) const
    {
        if (lhs.expired() || rhs.expired()) {
            return lhs.expired() && rhs.expired();
        }

        return lhs.lock().get() == rhs.lock().get();
    }
};

}

template <typename K, typename V>
class WeakPtrMap
    : public std::unordered_map<
          std::weak_ptr<K>, V, detail::WeakPtrHash<K>, detail::WeakPtrEq<K>> {
public:
    void clean(bool force = false)
    {
        if (!force) {
            const float lf = static_cast<float>(this->size() + 1)
                / static_cast<float>(this->bucket_count());

            force = lf >= this->max_load_factor();
        }

        if (force) {
            std::erase_if(
                *this, [](const auto& item) { return item.first.expired(); });
        }
    }
};

#endif /* A33C094A_DF9B_4BBB_B3B2_5AA3DD54CB65 */
