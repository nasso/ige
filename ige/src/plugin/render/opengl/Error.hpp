#ifndef BBDCCAB2_3239_4477_9B15_B71394AB4E61
#define BBDCCAB2_3239_4477_9B15_B71394AB4E61

#include "glad/gl.h"
#include <exception>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace gl {

class Error : public std::exception {
public:
    Error(GLenum code);

    const char* what() const override;

    static std::optional<Error> get();

#ifdef IGE_DEBUG
    static bool audit(std::string_view tag, std::ostream& out = std::cerr);
#else
    static constexpr bool audit(std::string_view, std::ostream& = std::cerr)
    {
        return false;
    }
#endif

private:
    GLenum m_code = 0;
};

}

#endif /* BBDCCAB2_3239_4477_9B15_B71394AB4E61 */
