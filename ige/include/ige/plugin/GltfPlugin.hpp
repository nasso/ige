#ifndef CDFA8719_13AD_4DC2_92B4_53B763ECF9D4
#define CDFA8719_13AD_4DC2_92B4_53B763ECF9D4

#include "ige/asset/AnimationClip.hpp"
#include "ige/core/App.hpp"
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace ige::plugin::gltf {

enum class GltfFormat { TEXT, BINARY };

class GltfScene {
public:
    GltfScene(std::string uri, GltfFormat format);

    void set_loaded(bool loaded = true);
    bool has_loaded() const;

    const std::string& uri() const;
    GltfFormat format() const;

private:
    bool m_loaded = false;
    std::string m_uri;
    GltfFormat m_format;
};

class GltfPlugin : public core::App::Plugin {
public:
    void plug(core::App::Builder&) const override;
};

}

#endif /* CDFA8719_13AD_4DC2_92B4_53B763ECF9D4 */
