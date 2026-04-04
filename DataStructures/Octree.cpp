#include "Octree.h"

#include "Box.h"
#include "Components/BoundingBox.h"
#include "Components/BoundingSphere.h"
#include "Components/MeshNodeTag.h"
#include "Components/OctreeLocation.h"
#include "Components/Transforms/WorldTransform.h"

#include "entt/entity/registry.hpp"
#include "glm/common.hpp"

namespace Octree {

namespace {

/**
 * \brief Preallocates an octree down to a specific depth.
 * \param center The center of the root node.
 * \param halfWidth The half width of the root node.
 * \param stopDepth The stop depth.
 * \return A pointer to the root node.
 */
std::unique_ptr<Node> BuildOctreeNodes(const glm::vec3 center, const float halfWidth, const int32_t stopDepth) {
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
        node->Children[i] = BuildOctreeNodes(center + offset, step, stopDepth - 1);
        if (node->Children[i]) {
            node->Children[i]->Parent = node.get();
        }
    }

    return node;
}

bool ContainsBBox(const Node* const node, const Box& box) {
    if (const Box octreeNodeBox{node->Center, node->HalfWidth}; !octreeNodeBox.Intersect(box)) {
        return true;
    }

    return false;
}

}

std::unique_ptr<Node> Build(entt::registry& registry) {
    Box sceneBoundingBox{glm::vec3{std::numeric_limits<float>::max()}, glm::vec3{std::numeric_limits<float>::lowest()}};

    const auto boundingBoxView{registry.view<BoundingBox, WorldTransform>()};
    for (const auto entity : boundingBoxView) {
        const auto& boundingBox{registry.get<BoundingBox>(entity).Box};
        const auto& worldTransform{registry.get<WorldTransform>(entity).Value};

        auto boundingBoxVertices{boundingBox.GetVertices()};
        std::ranges::for_each(boundingBoxVertices, [&worldTransform = std::as_const(worldTransform), &sceneBoundingBox](const glm::vec3 vertex) {
            const auto worldSpaceVertex{worldTransform.TransformPosition(vertex)};
            sceneBoundingBox.Min = glm::min(sceneBoundingBox.Min, worldSpaceVertex);
            sceneBoundingBox.Max = glm::max(sceneBoundingBox.Max, worldSpaceVertex);
        });
    }

    const auto size{sceneBoundingBox.GetSize()};
    const auto width{glm::max(size.x, glm::max(size.y, size.z))};
    auto rootNode{BuildOctreeNodes(sceneBoundingBox.GetCenter(), width * 0.5f, 1)};

    const auto meshNodeView{registry.view<MeshNodeTag>()};
    for (const auto meshEntity : meshNodeView) {
        // Each mesh that participates in the octree should save the pointer to the nodes it is contained.
        registry.emplace<OctreeLocation>(meshEntity);
        InsertEntity(rootNode.get(), meshEntity, registry);
    }

    return rootNode;
}

void InsertEntity(Node* const node, const entt::entity entity, entt::registry& registry) {
    assert(node);
    const auto& [center, radius]{registry.get<BoundingSphere>(entity)};
    const auto& worldTransform{registry.get<WorldTransform>(entity).Value};

    const auto worldSpaceCenter{worldTransform.TransformPosition(center)};
    const auto worldSpaceRadius{worldTransform.Scale * radius};

    int32_t index{0}, straddle{0};
    // Compute the octant number [0, 7] the entity bounding sphere is in.
    // If straddling any of the dividing x, y, z planes, exit.
    for (int32_t i{0}; i < 3; ++i) {
        // For each coordinate check if there is a straddling between the node and the geometry.
        const float delta{worldSpaceCenter[i] - node->Center[i]};
        if (std::abs(delta) < node->HalfWidth + worldSpaceRadius) {
            straddle = 1;
            break;
        }

        if (delta > 0.f) {
            index |= 1 << i;
        }
    }

    if (!straddle && node->Children[index]) {
        node->Children[index]->Parent = node;
        // TODO: maybe add here that if a child is null it gets created
        InsertEntity(node->Children[index].get(), entity, registry);
    } else {
        // If the entity is straddling in any of the dividing axes, add the entity on the current node.
        node->Entities.push_back(entity);
    }
}

void Update(const entt::entity entity, entt::registry& registry) {
    const auto& localSpaceBoundingBox{registry.get<BoundingBox>(entity).Box};
    const auto& worldTransform{registry.get<WorldTransform>(entity).Value};
    const Box entityWorldSpaceBoundingBox{worldTransform.TransformPosition(localSpaceBoundingBox.Min), worldTransform.TransformPosition(localSpaceBoundingBox.Max)};

    auto* const oldNode{registry.get<OctreeLocation>(entity).Node};
    auto* octreeNode{oldNode};
    assert(octreeNode);
    while (!ContainsBBox(octreeNode, entityWorldSpaceBoundingBox)) {
        // If we are at the root node, exit the loop and use the root node as starting point to insert the new entity.
        if (!octreeNode->Parent) {
            break;
        }

        octreeNode = octreeNode->Parent;
    }

    const auto removedElementsNum{oldNode->Entities.remove(entity)};
    assert(removedElementsNum == 1);

    InsertEntity(octreeNode, entity, registry);
}

} // namespace Octree
