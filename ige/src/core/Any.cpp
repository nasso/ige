
#include "ige/core/Any.hpp"

namespace ige {
namespace core {

    Any::Any(void* data, std::function<void(void*)> dtor)
        : m_data(data)
        , m_dtor(dtor)
    {
    }

    Any::Any(Any&& other)
        : m_data(other.m_data)
        , m_dtor(std::move(other.m_dtor))
    {
        other.m_data = nullptr;
    }

    Any::~Any()
    {
        if (m_data) {
            m_dtor(m_data);
        }

        m_data = nullptr;
    }

}
}
