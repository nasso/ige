#ifndef B95ECE62_B7EE_424D_8BD3_422F4D6BF32B
#define B95ECE62_B7EE_424D_8BD3_422F4D6BF32B

#include "igepch.hpp"

#include "glad/gl.h"

namespace gl {

class Shader {
public:
    enum ShaderType : GLenum {
        VERTEX = GL_VERTEX_SHADER,
        FRAGMENT = GL_FRAGMENT_SHADER,
    };

    class CompileError : public std::runtime_error {
    public:
        CompileError(const std::string& info_log);
    };

    Shader(ShaderType type, std::string_view source);
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&&);
    Shader& operator=(Shader&&);
    ~Shader();

    GLuint id() const;

private:
    GLuint m_id = 0;
};

}

#endif /* B95ECE62_B7EE_424D_8BD3_422F4D6BF32B */
