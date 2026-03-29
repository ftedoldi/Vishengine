#include "Octree.h"

#include "Box.h"
#include "Components/BoundingSphere.h"
#include "Components/MeshNodeTag.h"

#include "entt/entity/registry.hpp"
#include "glm/common.hpp"

namespace Octree {

    std::unique_ptr<Node> Build(entt::registry& registry) {
        const auto boundingSphereView{registry.view<BoundingSphere>()};

        Box sceneBoundingBox{.Min = glm::vec3{std::numeric_limits<float>::max()}, .Max = glm::vec3{std::numeric_limits<float>::lowest()}};

        for (const auto entity  : boundingSphereView) {
            const auto& boundingSphere{registry.get<BoundingSphere>(entity)};
            const auto boundingSphereWorldCenter{boundingSphere.WorldCenter};
            const auto boundingSphereWorldRadius{boundingSphere.WorldRadius};
            sceneBoundingBox.Min = glm::min(sceneBoundingBox.Min, boundingSphereWorldCenter - glm::vec3(boundingSphereWorldRadius));
            sceneBoundingBox.Max = glm::max(sceneBoundingBox.Max, boundingSphereWorldCenter + glm::vec3(boundingSphereWorldRadius));
        }

        // 2. Compute center of the AABB.
        glm::vec3 sceneBoundingBoxCenter{(sceneBoundingBox.Min + sceneBoundingBox.Max) * 0.5f};

        // 3. Half-width = half of the LARGEST axis extent (to make a cube).
        const glm::vec3 extents{(sceneBoundingBox.Max - sceneBoundingBox.Min) * 0.5f};
        float halfWidth{glm::max(extents.x, glm::max(extents.y, extents.z))};

        auto rootNode{BuildOctree(sceneBoundingBoxCenter, halfWidth, 1)};

        const auto meshNodeView{registry.view<MeshNodeTag>()};
        for (const auto meshEntity : meshNodeView) {
            InsertEntity(rootNode.get(), meshEntity, registry);
        }

        return rootNode;
    }

    std::unique_ptr<Node> BuildOctree(const glm::vec3 center, const float halfWidth, const int32_t stopDepth) {
        if (stopDepth < 0) {
            return nullptr;
        }

        auto node{std::make_unique<Node>(center, halfWidth)};

        glm::vec3 offset{};
        const float step{halfWidth * 0.5f};

        // Build the 8 octant children nodes.
        for (int i{0}; i < 8; ++i) {
            offset.x = ((i & 1) ? step : -step);
            offset.y = ((i & 2) ? step : -step);
            offset.z = ((i & 4) ? step : -step);
            node->Children[i] = BuildOctree(center + offset, halfWidth, stopDepth - 1);
        }

        return node;
    }

    void InsertEntity(Node* const node, const entt::entity entity, entt::registry& registry) {
        assert(node);
        const auto& boundingSphere{registry.get<BoundingSphere>(entity)};

        int32_t index{0}, straddle{0};
        // Compute the octant number [0, 7] the entity bounding sphere is in.
        // If straddling any of the dividing x, y, z planes, exit.
        for (int32_t i{0}; i < 3; ++i) {
            // For each coordinate check if there is a straddling between the node and the geometry.
            const float delta{boundingSphere.WorldCenter[i] - node->Center[i]};
            if (std::abs(delta) < node->HalfWidth + boundingSphere.WorldRadius) {
                straddle = 1;
                break;
            }

            if (delta > 0.f) {
                index |= 1 << i;
            }
        }

        if (!straddle && node->Children[index]) {
            // TODO: maybe add here that if a child is null it gets created
            InsertEntity(node->Children[index].get(), entity, registry);
        } else {
            // If the entity is straddling in any of the dividing axes, add the entity on the current node.
            node->EntityList.push_back(entity);
        }
    }

}


