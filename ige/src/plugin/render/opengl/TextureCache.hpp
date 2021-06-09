#ifndef D8D3249C_37C1_45C7_9002_B3E8DFB69A6A
#define D8D3249C_37C1_45C7_9002_B3E8DFB69A6A

#include "Texture.hpp"
#include "ige/asset/Texture.hpp"

struct TextureCache {
    gl::Texture gl_texture;

    TextureCache(const ige::asset::Texture& texture);
    ~TextureCache();
};

#endif /* D8D3249C_37C1_45C7_9002_B3E8DFB69A6A */
