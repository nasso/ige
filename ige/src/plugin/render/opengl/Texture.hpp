#ifndef ED0F4953_C3BE_4C2E_A2BA_C10B4A08B3C8
#define ED0F4953_C3BE_4C2E_A2BA_C10B4A08B3C8

#include "glad/gl.h"

namespace gl {

class Texture {
public:
    enum class Target : GLenum {
        TEXTURE_2D = GL_TEXTURE_2D,
    };

    enum class InternalFormat : GLenum {
        DEPTH_COMPONENT = GL_DEPTH_COMPONENT,
        DEPTH_STENCIL = GL_DEPTH_STENCIL,
        DEPTH24_STENCIL8 = GL_DEPTH24_STENCIL8,
        RED = GL_RED,
        RG = GL_RG,
        RGB = GL_RGB,
        RGBA = GL_RGBA,
        R8 = GL_R8,
        R8_SNORM = GL_R8_SNORM,
        R16 = GL_R16,
        R16_SNORM = GL_R16_SNORM,
        RG8 = GL_RG8,
        RG8_SNORM = GL_RG8_SNORM,
        RG16 = GL_RG16,
        RG16_SNORM = GL_RG16_SNORM,
        R3_G3_B2 = GL_R3_G3_B2,
        RGB4 = GL_RGB4,
        RGB5 = GL_RGB5,
        RGB8 = GL_RGB8,
        RGB8_SNORM = GL_RGB8_SNORM,
        RGB10 = GL_RGB10,
        RGB12 = GL_RGB12,
        RGB16_SNORM = GL_RGB16_SNORM,
        RGBA2 = GL_RGBA2,
        RGBA4 = GL_RGBA4,
        RGB5_A1 = GL_RGB5_A1,
        RGBA8 = GL_RGBA8,
        RGBA8_SNORM = GL_RGBA8_SNORM,
        RGB10_A2 = GL_RGB10_A2,
        RGB10_A2UI = GL_RGB10_A2UI,
        RGBA12 = GL_RGBA12,
        RGBA16 = GL_RGBA16,
        SRGB8 = GL_SRGB8,
        SRGB8_ALPHA8 = GL_SRGB8_ALPHA8,
        R16F = GL_R16F,
        RG16F = GL_RG16F,
        RGB16F = GL_RGB16F,
        RGBA16F = GL_RGBA16F,
        R32F = GL_R32F,
        RG32F = GL_RG32F,
        RGB32F = GL_RGB32F,
        RGBA32F = GL_RGBA32F,
        R11F_G11F_B10F = GL_R11F_G11F_B10F,
        RGB9_E5 = GL_RGB9_E5,
        R8I = GL_R8I,
        R8UI = GL_R8UI,
        R16I = GL_R16I,
        R16UI = GL_R16UI,
        R32I = GL_R32I,
        R32UI = GL_R32UI,
        RG8I = GL_RG8I,
        RG8UI = GL_RG8UI,
        RG16I = GL_RG16I,
        RG16UI = GL_RG16UI,
        RG32I = GL_RG32I,
        RG32UI = GL_RG32UI,
        RGB8I = GL_RGB8I,
        RGB8UI = GL_RGB8UI,
        RGB16I = GL_RGB16I,
        RGB16UI = GL_RGB16UI,
        RGB32I = GL_RGB32I,
        RGB32UI = GL_RGB32UI,
        RGBA8I = GL_RGBA8I,
        RGBA8UI = GL_RGBA8UI,
        RGBA16I = GL_RGBA16I,
        RGBA16UI = GL_RGBA16UI,
        RGBA32I = GL_RGBA32I,
        RGBA32UI = GL_RGBA32UI,
    };

    enum class Format : GLenum {
        RED = GL_RED,
        RG = GL_RG,
        RGB = GL_RGB,
        BGR = GL_BGR,
        RGBA = GL_RGBA,
        BGRA = GL_BGRA,
        DEPTH_COMPONENT = GL_DEPTH_COMPONENT,
        DEPTH_STENCIL = GL_DEPTH_STENCIL,
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

    GLuint id() const;

    static void image_2d(
        Target, GLint level, InternalFormat, GLsizei w, GLsizei h, Format, Type,
        const void* data);
    static void filter(Target, MagFilter, MinFilter);
    static void wrap(Target, Wrap s, Wrap t);
    static void gen_mipmaps(Target);

    static void bind(Target, const Texture&);
    static void unbind(Target);

private:
    GLuint m_id = 0;
};

}

#endif /* ED0F4953_C3BE_4C2E_A2BA_C10B4A08B3C8 */
