#ifndef B9861FCE_54F8_45FA_9DDB_2BDD0F68CD1A
#define B9861FCE_54F8_45FA_9DDB_2BDD0F68CD1A

#include "igepch.hpp"

#include "Shader.hpp"
#include "Texture.hpp"
#include "glad/gl.h"

namespace gl {

class Program {
public:
    class LinkError : public std::runtime_error {
    public:
        LinkError(const char* info_log);
    };

    Program();
    Program(const Program&) = delete;
    Program& operator=(const Program&) = delete;
    Program(Program&&);
    Program& operator=(Program&&);
    ~Program();

    Program(const Shader& vs, const Shader& fs);

    void attach(const Shader&);
    void detach(const Shader&);
    void link(const Shader&, const Shader&);
    void link();
    void use() const;
    GLuint id() const;

    GLuint uniform(const char* name) const;
    GLuint uniform_block(const char* name) const;

    void uniform(const char* name, int);
    void uniform(const char* name, std::span<const int>);
    void uniform(const char* name, float);
    void uniform(const char* name, std::span<const float>);
    void uniform(const char* name, const glm::vec2&);
    void uniform(const char* name, std::span<const glm::vec2>);
    void uniform(const char* name, const glm::vec3&);
    void uniform(const char* name, std::span<const glm::vec3>);
    void uniform(const char* name, const glm::vec4&);
    void uniform(const char* name, std::span<const glm::vec4>);
    void uniform(const char* name, const glm::mat2&);
    void uniform(const char* name, std::span<const glm::mat2>);
    void uniform(const char* name, const glm::mat3&);
    void uniform(const char* name, std::span<const glm::mat3>);
    void uniform(const char* name, const glm::mat4&);
    void uniform(const char* name, std::span<const glm::mat4>);

    void uniform_block(const char* name, GLuint uniform_block_binding);

private:
    GLuint m_id = 0;
};

}

#endif /* B9861FCE_54F8_45FA_9DDB_2BDD0F68CD1A */
