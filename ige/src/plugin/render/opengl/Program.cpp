#include "Program.hpp"
#include "Shader.hpp"
#include "glad/gl.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

using gl::Program;
using gl::Shader;
using gl::Texture;
using glm::mat2;
using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

Program::LinkError::LinkError(const std::string& info_log)
    : std::runtime_error(info_log)
{
}

Program::Program()
{
    m_id = glCreateProgram();
}

Program::Program(const Shader& vs, const Shader& fs)
    : Program()
{
    link(vs, fs);
}

Program::Program(Program&& other)
{
    *this = std::move(other);
}

Program& Program::operator=(Program&& other)
{
    if (m_id) {
        glDeleteProgram(m_id);
    }

    m_id = other.m_id;
    other.m_id = 0;
    return *this;
}

Program::~Program()
{
    if (m_id) {
        glDeleteProgram(m_id);
    }
}

void Program::attach(const Shader& shader)
{
    glAttachShader(m_id, shader.id());
}

void Program::detach(const Shader& shader)
{
    glDetachShader(m_id, shader.id());
}

void Program::link()
{
    glLinkProgram(m_id);

    GLint result = 0;

    glGetProgramiv(m_id, GL_LINK_STATUS, &result);

    if (!result) {
        std::string log;

        GLint log_length = 0;
        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &log_length);

        if (log_length > 0) {
            std::vector<char> logv(log_length);

            glGetProgramInfoLog(m_id, log_length, nullptr, logv.data());

            log.assign(logv.begin(), logv.end());
        }

        glDeleteProgram(m_id);
        throw Program::LinkError(log);
    }
}

void Program::link(const Shader& vs, const Shader& fs)
{
    attach(vs);
    attach(fs);

    try {
        link();
    } catch (...) {
        detach(vs);
        detach(fs);
        throw;
    }

    detach(vs);
    detach(fs);
}

void Program::use() const
{
    glUseProgram(m_id);
}

GLuint Program::id() const
{
    return m_id;
}

GLuint Program::uniform(const std::string& name)
{
    return glGetUniformLocation(m_id, name.c_str());
}

void Program::uniform(const std::string& name, int value)
{
    use();
    glUniform1i(uniform(name), value);
}

void Program::uniform(const std::string& name, std::span<const int> values)
{
    if (values.empty()) {
        return;
    }

    use();
    glUniform1iv(
        uniform(name), static_cast<GLsizei>(values.size()),
        reinterpret_cast<const GLint*>(values.data()));
}

void Program::uniform(const std::string& name, float value)
{
    use();
    glUniform1f(uniform(name), value);
}

void Program::uniform(const std::string& name, std::span<const float> values)
{
    if (values.empty()) {
        return;
    }

    use();
    glUniform1fv(
        uniform(name), static_cast<GLsizei>(values.size()), values.data());
}

void Program::uniform(const std::string& name, const vec2& value)
{
    use();
    glUniform2f(uniform(name), value.x, value.y);
}

void Program::uniform(const std::string& name, std::span<const vec2> values)
{
    if (values.empty()) {
        return;
    }

    use();
    glUniform2fv(
        uniform(name), static_cast<GLsizei>(values.size()),
        reinterpret_cast<const GLfloat*>(values.data()));
}

void Program::uniform(const std::string& name, const vec3& value)
{
    use();
    glUniform3f(uniform(name), value.x, value.y, value.z);
}

void Program::uniform(const std::string& name, std::span<const vec3> values)
{
    if (values.empty()) {
        return;
    }

    use();
    glUniform3fv(
        uniform(name), static_cast<GLsizei>(values.size()),
        reinterpret_cast<const GLfloat*>(values.data()));
}

void Program::uniform(const std::string& name, const vec4& value)
{
    use();
    glUniform4f(uniform(name), value.x, value.y, value.z, value.w);
}

void Program::uniform(const std::string& name, std::span<const vec4> values)
{
    if (values.empty()) {
        return;
    }

    use();
    glUniform4fv(
        uniform(name), static_cast<GLsizei>(values.size()),
        reinterpret_cast<const GLfloat*>(values.data()));
}

void Program::uniform(const std::string& name, const mat2& value)
{
    uniform(name, std::span(&value, 1));
}

void Program::uniform(const std::string& name, std::span<const mat2> values)
{
    if (values.empty()) {
        return;
    }

    use();
    glUniformMatrix2fv(
        uniform(name), static_cast<GLsizei>(values.size()), GL_FALSE,
        reinterpret_cast<const GLfloat*>(glm::value_ptr(values.front())));
}

void Program::uniform(const std::string& name, const mat3& value)
{
    uniform(name, std::span(&value, 1));
}

void Program::uniform(const std::string& name, std::span<const mat3> values)
{
    if (values.empty()) {
        return;
    }

    use();
    glUniformMatrix3fv(
        uniform(name), static_cast<GLsizei>(values.size()), GL_FALSE,
        reinterpret_cast<const GLfloat*>(glm::value_ptr(values.front())));
}

void Program::uniform(const std::string& name, const mat4& value)
{
    uniform(name, std::span(&value, 1));
}

void Program::uniform(const std::string& name, std::span<const mat4> values)
{
    if (values.empty()) {
        return;
    }

    use();
    glUniformMatrix4fv(
        uniform(name), static_cast<GLsizei>(values.size()), GL_FALSE,
        reinterpret_cast<const GLfloat*>(glm::value_ptr(values.front())));
}

void Program::uniform(const std::string& name, const Texture& tex)
{
    use();
    tex.bind();
    glUniform1i(uniform(name), 0);
}
