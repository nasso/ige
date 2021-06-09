#ifndef ED0F4953_C3BE_4C2E_A2BA_C10B4A08B3C8
#define ED0F4953_C3BE_4C2E_A2BA_C10B4A08B3C8

#include "glad/gl.h"

namespace gl {

class Texture {
private:
    GLuint m_id = 0;

public:
    enum class Format : GLenum {
        BGRA = GL_BGRA,
        RGBA = GL_RGBA,
        RGB = GL_RGB,
        BGR = GL_BGR,
        RG = GL_RG,
        RED = GL_RED,
    };

    enum class Type : GLenum {
        UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
        FLOAT = GL_FLOAT,
    };

    enum class MagFilter : GLenum {
        NEAREST = GL_NEAREST,
        LINEAR = GL_LINEAR,
    };

    enum class MinFilter : GLenum {
        NEAREST = GL_NEAREST,
        NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
        NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
        LINEAR = GL_LINEAR,
        LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
        LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
    };

    enum class Wrap : GLenum {
        REPEAT = GL_REPEAT,
        CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
        MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
    };

    Texture();
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other);
    Texture& operator=(Texture&& other);
    ~Texture();

    void
    load_pixels(GLsizei w, GLsizei h, Format fmt, Type type, const void* data);

    void bind() const;
    GLuint id() const;

    void filter(MagFilter mag, MinFilter min);
    void wrap(Wrap s, Wrap t);
    void gen_mipmaps();
};

}

#endif /* ED0F4953_C3BE_4C2E_A2BA_C10B4A08B3C8 */
