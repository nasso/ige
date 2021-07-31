#include "World.hpp"

namespace ige::core {

template <Component... Cs>
Entity World::spawn(Cs... comps)
{
    // TODO

    return 0;
}

template <Plugin P>
bool World::load()
{
    // TODO: avoid loading twice
    spawn(P(*this));

    return true;
}

}
