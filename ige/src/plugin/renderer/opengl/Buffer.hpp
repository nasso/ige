#ifndef EECC3E28_49AF_4009_90BE_066F9860D4F6
#define EECC3E28_49AF_4009_90BE_066F9860D4F6

#include "glad/gl.h"
#include <cstddef>
#include <span>
#include <vector>

namespace ige {
namespace gl {

    class Buffer {
    private:
        GLenum m_target = GL_ARRAY_BUFFER;
        GLuint m_id = 0;

    public:
        Buffer(GLenum target = GL_ARRAY_BUFFER);
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;
        Buffer(Buffer&& other);
        Buffer& operator=(Buffer&& other);
        ~Buffer();

        template <typename T>
        void load(std::span<const T> data)
        {
            bind();
            glBufferData(
                m_target, data.size_bytes(), data.data(), GL_STATIC_DRAW);
        }

        void bind() const;
        GLuint id() const;
    };

}
}

#endif /* EECC3E28_49AF_4009_90BE_066F9860D4F6 */
