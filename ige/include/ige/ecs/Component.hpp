#ifndef A0057E91_19D4_4C57_A362_8A5B53D85BDE
#define A0057E91_19D4_4C57_A362_8A5B53D85BDE

#include <concepts>

namespace ige {
namespace ecs {

    template <typename T>
    concept Component = std::movable<T>;

}
}

#endif /* A0057E91_19D4_4C57_A362_8A5B53D85BDE */
