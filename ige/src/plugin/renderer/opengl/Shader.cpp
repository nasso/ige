#include "Shader.hpp"
#include "glad/gl.h"
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

using ige::gl::Shader;

Shader::Shader(Shader::ShaderType type, std::string_view source)
{
    GLint len = static_cast<GLint>(source.size());
    const char* src = source.data();

    m_id = glCreateShader(type);
    glShaderSource(m_id, 1, &src, &len);
    glCompileShader(m_id);

    GLint result = 0;

    glGetShaderiv(m_id, GL_COMPILE_STATUS, &result);

    if (!result) {
        std::string log;

        GLint log_length = 0;
        glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &log_length);

        if (log_length > 0) {
            std::vector<char> logv(log_length);

            glGetShaderInfoLog(m_id, log_length, nullptr, logv.data());

            log.assign(logv.begin(), logv.end());
        }

        glDeleteShader(m_id);
        throw Shader::CompileError(log);
    }
}

Shader::Shader(Shader&& other)
{
    *this = std::move(other);
}

Shader& Shader::operator=(Shader&& other)
{
    if (m_id) {
        glDeleteShader(m_id);
    }

    m_id = other.m_id;
    other.m_id = 0;
    return *this;
}

Shader::~Shader()
{
    if (m_id) {
        glDeleteShader(m_id);
    }
}

GLuint Shader::id() const
{
    return m_id;
}

Shader::CompileError::CompileError(const std::string& info_log)
    : std::runtime_error(info_log)
{
}
