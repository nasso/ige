#ifndef F706145A_9A19_4133_998F_05F53EAFD8B9
#define F706145A_9A19_4133_998F_05F53EAFD8B9

#include <cstddef>
#include <glm/mat4x4.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace ige::asset {

struct SkeletonJoint {
    glm::mat4 inv_bind_matrix;
    std::optional<std::size_t> parent;
};

struct Skeleton {
    using Handle = std::shared_ptr<Skeleton>;

    std::vector<SkeletonJoint> joints;
};

}

#endif /* F706145A_9A19_4133_998F_05F53EAFD8B9 */
