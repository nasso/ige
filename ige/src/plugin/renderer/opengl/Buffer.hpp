#ifndef EECC3E28_49AF_4009_90BE_066F9860D4F6
#define EECC3E28_49AF_4009_90BE_066F9860D4F6

#include "glad/gl.h"
#include <cstddef>
#include <vector>

namespace ige {
namespace gl {

    class Buffer {
    private:
        GLuint m_id = 0;

    public:
        Buffer();
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;
        Buffer(Buffer&& other);
        Buffer& operator=(Buffer&& other);
        ~Buffer();

        template <typename T>
        void load_static(const T* data, std::size_t len)
        {
            bind();
            glBufferData(
                GL_ARRAY_BUFFER, sizeof(T) * len, data, GL_STATIC_DRAW);
        }

        void bind() const;
        GLuint id() const;
    };

}
}

#endif /* EECC3E28_49AF_4009_90BE_066F9860D4F6 */
