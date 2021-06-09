#ifndef BBDCCAB2_3239_4477_9B15_B71394AB4E61
#define BBDCCAB2_3239_4477_9B15_B71394AB4E61

#include "glad/gl.h"
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace gl {

class Error : public std::runtime_error {
private:
    Error(const std::string& msg);

public:
    Error(GLenum code);

    static std::optional<Error> get();
    static std::vector<Error> get_all();
    static bool audit(const std::string& tag, std::ostream& out = std::cerr);
    static void expect_none(const std::string& tag);
};

}

#endif /* BBDCCAB2_3239_4477_9B15_B71394AB4E61 */
