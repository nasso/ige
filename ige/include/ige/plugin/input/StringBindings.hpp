#ifndef D89DA03D_0323_4547_ADAD_68DC1F3D39D0
#define D89DA03D_0323_4547_ADAD_68DC1F3D39D0

#include "Bindings.hpp"
#include <functional>
#include <string>

namespace ige::plugin::input {

template <>
class BindingIdentifier<std::string> {
public:
    std::optional<std::string> from_json(const nlohmann::json& src)
    {
        if (src.is_string()) {
            return src.get<std::string>();
        }
        return {};
    }

    nlohmann::json to_json(const std::string& src)
    {
        return src;
    }
};

}

#endif /* D89DA03D_0323_4547_ADAD_68DC1F3D39D0 */
