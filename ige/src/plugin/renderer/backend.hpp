#ifndef EA0C3563_AB58_4A44_900B_8D737ECE36A0
#define EA0C3563_AB58_4A44_900B_8D737ECE36A0

#include "ige/ecs/World.hpp"

namespace ige::plugin::render::backend {

void clear_buffers(ecs::World&);
void render_meshes(ecs::World&);
void clear_cache(ecs::World&);

}

#endif /* EA0C3563_AB58_4A44_900B_8D737ECE36A0 */
