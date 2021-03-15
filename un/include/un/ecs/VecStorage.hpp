/*
** EPITECH PROJECT, 2021
** un
** File description:
** VecStorage
*/

#ifndef DB322AB1_C834_42D3_82D6_5F31BCC83A33
#define DB322AB1_C834_42D3_82D6_5F31BCC83A33

#include "rtl/Option.hpp"
#include <vector>

namespace un {
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

        template <typename... Args,
            typename = std::enable_if_t<
                std::is_constructible<T, Args...>::value>>
        void set(std::size_t idx, Args&&... args)
        {
            rtl::Option<T> opt;

            opt.replace(std::forward<Args>(args)...);

            m_data.resize(idx + 1);
            m_data.emplace(m_data.begin() + idx, std::move(opt));
        }

        rtl::Option<const T&> get(const std::size_t& idx) const
        {
            if (idx < m_data.size()) {
                return m_data[idx].as_ref();
            } else {
                return {};
            }
        }

        rtl::Option<T&> get(const std::size_t& idx)
        {
            if (idx < m_data.size()) {
                return m_data[idx].as_mut();
            } else {
                return {};
            }
        }

        rtl::Option<T> remove(const std::size_t& idx)
        {
            if (idx < m_data.size()) {
                return m_data[idx].take();
            } else {
                return {};
            }
        }

    private:
        std::vector<rtl::Option<T>> m_data;
    };

}
}

#endif /* DB322AB1_C834_42D3_82D6_5F31BCC83A33 */
