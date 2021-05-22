#ifndef B9861FCE_54F8_45FA_9DDB_2BDD0F68CD1A
#define B9861FCE_54F8_45FA_9DDB_2BDD0F68CD1A

#include "Shader.hpp"
#include "Texture.hpp"
#include "glad/gl.h"
#include <cmath>
#include <stdexcept>
#include <string>

#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
; // TODO: https://bit.ly/3hhMJ58

namespace ige {
namespace gl {

    class Program {
    public:
        class LinkError : public std::runtime_error {
        public:
            LinkError(const std::string& info_log);
        };

        Program(const Shader& vs, const Shader& fs);
        Program(const Program&) = delete;
        Program& operator=(const Program&) = delete;
        Program(Program&&);
        Program& operator=(Program&&);
        ~Program();

        void use() const;
        GLuint id() const;

        GLuint uniform(const std::string& name);
        void uniform(const std::string& name, bool);
        void uniform(const std::string& name, float);
        void uniform(const std::string& name, const glm::vec2&);
        void uniform(const std::string& name, const glm::vec3&);
        void uniform(const std::string& name, const glm::vec4&);
        void uniform(const std::string& name, const glm::mat2&);
        void uniform(const std::string& name, const glm::mat3&);
        void uniform(const std::string& name, const glm::mat4&);
        void uniform(const std::string& name, const Texture&);

    private:
        GLuint m_id = 0;
    };

}
}

#endif /* B9861FCE_54F8_45FA_9DDB_2BDD0F68CD1A */
