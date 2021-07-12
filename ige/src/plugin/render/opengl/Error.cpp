#include "igepch.hpp"

#include "Error.hpp"
#include "glad/gl.h"

using gl::Error;

Error::Error(GLenum code)
    : m_code(code)
{
}

const char* Error::what() const noexcept
{
    switch (m_code) {
    case GL_NO_ERROR:
        return "GL_NO_ERROR";
    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";
    default:
        return "?";
    }
}

#ifdef IGE_DEBUG

std::optional<Error> Error::get()
{
    GLenum err = glGetError();

    if (err) {
        return Error(err);
    } else {
        return std::nullopt;
    }
}

bool Error::audit(std::string_view tag, std::ostream& out)
{
    auto error = Error::get();

    if (error) {
        out << "Caught GL error(s) at \"" << tag << "\":\n";

        for (std::size_t i = 0; error; i++) {
            out << i << ". " << error->what() << "\n";
            error = Error::get();
        }

        return true;
    }

    return false;
}

#else /* NOT IGE_DEBUG */

std::optional<Error> Error::get()
{
    return std::nullopt;
}

#endif
