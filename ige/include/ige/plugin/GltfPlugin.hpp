#ifndef CDFA8719_13AD_4DC2_92B4_53B763ECF9D4
#define CDFA8719_13AD_4DC2_92B4_53B763ECF9D4

#include "ige/core/App.hpp"
#include <string>

namespace ige {
namespace plugin {
    namespace gltf {

        enum class GltfFormat {
            TEXT,
            BINARY,
        };

        struct GltfScene {
            std::string uri;
            GltfFormat format;

            GltfScene(std::string uri, GltfFormat format);

            bool operator==(const GltfScene&) const = default;
        };

        class GltfPlugin : public core::App::Plugin {
        public:
            void plug(core::App::Builder&) const override;
        };

    }
}
}

#endif /* CDFA8719_13AD_4DC2_92B4_53B763ECF9D4 */
