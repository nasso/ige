/*
** EPITECH PROJECT, 2021
** un
** File description:
** VecStorage
*/

#ifndef DB322AB1_C834_42D3_82D6_5F31BCC83A33
#define DB322AB1_C834_42D3_82D6_5F31BCC83A33

#include "IStorage.hpp"
#include "rtl/Option.hpp"
#include <vector>

namespace un {
namespace ecs {

    template <typename T>
    class VecStorage : public IStorage<std::size_t, T> {
    public:
        VecStorage() = default;
        virtual ~VecStorage() = default;

        virtual void set(std::size_t&& idx, T&& value) override
        {
            m_data.resize(idx + 1);
            m_data[idx] = rtl::some(value);
        }

        virtual rtl::Option<const T&> get(const std::size_t& idx) const override
        {
            if (idx < m_data.size()) {
                return m_data[idx].as_ref();
            } else {
                return {};
            }
        }

        virtual rtl::Option<T&> get(const std::size_t& idx) override
        {
            if (idx < m_data.size()) {
                return m_data[idx].as_mut();
            } else {
                return {};
            }
        }

        virtual rtl::Option<T> remove(const std::size_t& idx) override
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
