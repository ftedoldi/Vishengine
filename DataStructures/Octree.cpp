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

// Node boxes are 50% bigger
constexpr float LOOSE_FACTOR{1.5f};

void BuildOctreeNodes(Node* parentNode, const glm::vec3 center, const glm::vec3 extent) {
    glm::vec3 offset{};
    const auto step{extent * 0.5f};

    // Build the 8 octant children nodes.
    for (int i{0}; i < 8; ++i) {
        offset.x = ((i & 1) ? step.x : -step.x);
        offset.y = ((i & 2) ? step.y : -step.y);
        offset.z = ((i & 4) ? step.z : -step.z);
        auto node{std::make_unique<Node>(center + offset, step/* * LOOSE_FACTOR*/)};
        node->Parent = parentNode;
        parentNode->Children[i] = std::move(node);
    }
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

    auto rootNode{std::make_unique<Node>(sceneBoundingBox.GetCenter(), sceneBoundingBox.GetExtent()/* * LOOSE_FACTOR*/)};

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
    const auto& [localMin, localMax]{registry.get<BoundingBox>(entity).Box};
    const auto& worldTransform{registry.get<WorldTransform>(entity).Value};

    const auto worldSpaceMin{worldTransform.TransformPosition(localMin)};
    const auto worldSpaceMax{worldTransform.TransformPosition(localMax)};
    const Box worldSpaceBox{worldSpaceMin, worldSpaceMax};
    const auto worldSpaceCenter{worldSpaceBox.GetCenter()};
    const auto worldSpaceExtent{worldSpaceBox.GetExtent()};
    bool straddle{};
    int32_t index{};
    // Compute the octant number [0, 7] the entity bounding sphere is in.
    // If straddling any of the dividing x, y, z planes, exit.
    for (int32_t i{0}; i < 3; ++i) {
        // For each coordinate check if there is a straddling between the node and the geometry.
        const float delta{worldSpaceCenter[i] - node->Center[i]};
        if (std::abs(delta) < worldSpaceExtent[i] || std::abs(delta) + worldSpaceExtent[i] > node->HalfWidth[i]) {
            straddle = true;
            break;
        }

        if (delta > 0.f) {
            index |= 1 << i;
        }
    }

    if (!straddle) {
        // Se non sta straddlando significa che l'entity e' completamente contenuta in un nodo figlio
        // creo dunque gli 8 figli e richiamo questa funzione
        if (!node->Children[0]) {
            BuildOctreeNodes(node, node->Center, node->HalfWidth);
        }
        InsertEntity(node->Children[index].get(), entity, registry);
    } else {
        // If the entity is straddling in any of the dividing axes, add the entity on the current node.
        node->Entities.push_back(entity);
        registry.get<OctreeLocation>(entity).Node = node;
    }
}

void Update(const entt::entity entity, entt::registry& registry) {
    const auto& localSpaceBoundingBox{registry.get<BoundingBox>(entity).Box};
    const auto& worldTransform{registry.get<WorldTransform>(entity).Value};
    const Box entityWorldSpaceBoundingBox{worldTransform.TransformPosition(localSpaceBoundingBox.Min), worldTransform.TransformPosition(localSpaceBoundingBox.Max)};

    auto* const oldNode{registry.get<OctreeLocation>(entity).Node};
    auto* octreeNode{oldNode};
    assert(octreeNode);
    const Box octreeNodeBox{octreeNode->Center, octreeNode->HalfWidth};
    while (!octreeNodeBox.Contains(entityWorldSpaceBoundingBox)) {
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
