#ifndef EECC3E28_49AF_4009_90BE_066F9860D4F6
#define EECC3E28_49AF_4009_90BE_066F9860D4F6

#include "igepch.hpp"

#include "glad/gl.h"
#include <cstddef>

namespace gl {

class Buffer {
private:
    GLuint m_id = 0;

public:
    enum class Target : GLenum {
        ARRAY_BUFFER = GL_ARRAY_BUFFER,
        COPY_READ_BUFFER = GL_COPY_READ_BUFFER,
        COPY_WRITE_BUFFER = GL_COPY_WRITE_BUFFER,
        ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
        PIXEL_PACK_BUFFER = GL_PIXEL_PACK_BUFFER,
        PIXEL_UNPACK_BUFFER = GL_PIXEL_UNPACK_BUFFER,
        TEXTURE_BUFFER = GL_TEXTURE_BUFFER,
        TRANSFORM_FEEDBACK_BUFFER = GL_TRANSFORM_FEEDBACK_BUFFER,
        UNIFORM_BUFFER = GL_UNIFORM_BUFFER,
    };

    enum class Usage : GLenum {
        STREAM_DRAW = GL_STREAM_DRAW,
        STREAM_READ = GL_STREAM_READ,
        STREAM_COPY = GL_STREAM_COPY,
        STATIC_DRAW = GL_STATIC_DRAW,
        STATIC_READ = GL_STATIC_READ,
        STATIC_COPY = GL_STATIC_COPY,
        DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
        DYNAMIC_READ = GL_DYNAMIC_READ,
        DYNAMIC_COPY = GL_DYNAMIC_COPY,
    };

    enum class Access : GLenum {
        READ_ONLY = GL_READ_ONLY,
        WRITE_ONLY = GL_WRITE_ONLY,
        READ_WRITE = GL_READ_WRITE,
    };

    Buffer();
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&& other);
    Buffer& operator=(Buffer&& other);
    ~Buffer();

    GLuint id() const;

    static void bind(Target, const Buffer&);
    static void bind_base(Target, GLuint binding_point_index, const Buffer&);
    static void unbind(Target);
    static void data(Target, GLsizeiptr size, const void* data, Usage);
    static void
    sub_data(Target, GLintptr offset, GLsizeiptr size, const void* data);
    static void* map(Target, Access);
    static void* map(Target, GLintptr offset, GLsizeiptr length, Access);
    static GLboolean unmap(Target);

    template <typename T>
    static void data(
        Target target, std::span<const T> data,
        Usage usage = Usage::STATIC_DRAW)
    {
        Buffer::data(target, data.size_bytes(), data.data(), usage);
    }

    template <typename T>
    static void
    sub_data(Target target, GLintptr offset, std::span<const T> data)
    {
        Buffer::sub_data(target, offset, data.size_bytes(), data.data());
    }

    template <typename T>
    static void map_read(Target target, std::function<void(const T*)> fn)
    {
        fn(reinterpret_cast<const T*>(Buffer::map(target, Access::READ_ONLY)));
        Buffer::unmap(target);
    }

    template <typename T>
    static void map_write(Target target, std::function<void(T*)> fn)
    {
        fn(reinterpret_cast<T*>(Buffer::map(target, Access::WRITE_ONLY)));
        Buffer::unmap(target);
    }

    template <typename T>
    static void map_read_write(Target target, std::function<void(T*)> fn)
    {
        fn(reinterpret_cast<T*>(Buffer::map(target, Access::READ_WRITE)));
        Buffer::unmap(target);
    }
};

}

#endif /* EECC3E28_49AF_4009_90BE_066F9860D4F6 */
