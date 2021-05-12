#ifndef ED0F4953_C3BE_4C2E_A2BA_C10B4A08B3C8
#define ED0F4953_C3BE_4C2E_A2BA_C10B4A08B3C8

#include "glad/gl.h"

namespace ige {
namespace gl {

    class Texture {
    private:
        GLuint m_id = 0;

    public:
        enum Format : GLenum {
            BGRA = GL_BGRA,
            RGBA = GL_RGBA,
            RGB = GL_RGB,
            BGR = GL_BGR,
        };

        enum Type : GLenum {
            UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
        };

        enum Filter : GLenum {
            NEAREST = GL_NEAREST,
            LINEAR = GL_LINEAR,
        };

        enum Wrap : GLenum {
            REPEAT = GL_REPEAT,
            CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
            CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
            MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
        };

        Texture();
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&& other);
        Texture& operator=(Texture&& other);
        ~Texture();

        void load_pixels(
            GLsizei w, GLsizei h, Format fmt, Type type, const void* data);

        void bind() const;
        GLuint id() const;

        void filter(Filter mag, Filter min);
        void filter(Filter f);
        void wrap(Wrap s, Wrap t);
        void wrap(Wrap w);
    };

}
}

#endif /* ED0F4953_C3BE_4C2E_A2BA_C10B4A08B3C8 */
