#ifndef BBDCCAB2_3239_4477_9B15_B71394AB4E61
#define BBDCCAB2_3239_4477_9B15_B71394AB4E61

#include "igepch.hpp"

#include "glad/gl.h"

namespace gl {

class Error : public std::exception {
public:
    Error(GLenum code);

    const char* what() const noexcept override;

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
