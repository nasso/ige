#ifndef FF4C8D44_2751_4BEE_B440_F854C20FCFCD
#define FF4C8D44_2751_4BEE_B440_F854C20FCFCD

#include "Renderbuffer.hpp"
#include "Texture.hpp"
#include "glad/gl.h"
#include <cstddef>
#include <optional>
#include <span>
#include <vector>

namespace gl {

class Framebuffer {
public:
    enum class Target : GLenum {
        FRAMEBUFFER = GL_FRAMEBUFFER,
        READ_FRAMEBUFFER = GL_READ_FRAMEBUFFER,
        DRAW_FRAMEBUFFER = GL_DRAW_FRAMEBUFFER,
    };

    class Attachment {
    public:
        static Attachment COLOR(std::size_t);
        static Attachment DEPTH;
        static Attachment STENCIL;
        static Attachment DEPTH_STENCIL;

        explicit operator GLenum();

    private:
        Attachment(GLenum);

        GLenum m_value = 0;
    };

    Framebuffer();
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&& other);
    Framebuffer& operator=(Framebuffer&& other);
    ~Framebuffer();

    GLuint id() const;

    static void bind(Target, const Framebuffer& id);
    static void unbind(Target);
    static void attach(Target, Attachment, const Renderbuffer&);
    static void attach(Target, Attachment, const Texture&, GLint level = 0);

private:
    GLuint m_id = 0;
};

}

#endif /* FF4C8D44_2751_4BEE_B440_F854C20FCFCD */
