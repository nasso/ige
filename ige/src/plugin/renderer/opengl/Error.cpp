#include "Error.hpp"
#include "glad/gl.h"
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using ige::gl::Error;

const std::unordered_map<GLenum, std::string> ERROR_MAP = {
    { GL_NO_ERROR, "GL_NO_ERROR" },
    { GL_INVALID_ENUM, "GL_INVALID_ENUM" },
    { GL_INVALID_VALUE, "GL_INVALID_VALUE" },
    { GL_INVALID_OPERATION, "GL_INVALID_OPERATION" },
    { GL_INVALID_FRAMEBUFFER_OPERATION, "GL_INVALID_FRAMEBUFFER_OPERATION" },
    { GL_OUT_OF_MEMORY, "GL_OUT_OF_MEMORY" },
};

Error::Error(GLenum code)
    : std::runtime_error(ERROR_MAP.at(code))
{
}

Error::Error(const std::string& msg)
    : std::runtime_error(msg)
{
}

std::optional<Error> Error::get()
{
    GLenum err = glGetError();

    if (err) {
        return Error(err);
    } else {
        return {};
    }
}

std::vector<Error> Error::get_all()
{
    std::vector<Error> errors;

    while (auto err = Error::get()) {
        errors.push_back(*err);
    }

    return errors;
}

bool Error::audit(const std::string& tag, std::ostream& out)
{
    auto errors = Error::get_all();

    if (errors.size()) {
        out << "Caught GL error";

        if (errors.size() > 1) {
            out << "s";
        }

        out << " at \"" << tag << "\":\n";

        for (const auto& err : errors) {
            out << " - " << err.what() << "\n";
        }

        return true;
    }

    return false;
}

void Error::expect_none(const std::string& tag)
{
    std::stringstream ss;

    if (audit(tag, ss)) {
        throw Error(ss.str());
    }
}
