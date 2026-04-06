#include "Octree.h"

#include "Box.h"
#include "Components/BoundingBox.h"
#include "Components/MeshNodeTag.h"
#include "Components/OctreeLocation.h"
#include "Components/Transforms/WorldTransform.h"

#include "entt/entity/registry.hpp"
#include "glm/common.hpp"

namespace OC {

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

bool IsSubtreeEmpty(const Node* node) {
    if (!node) {
        return true;
    }

    if (!node->Entities.empty()) {
        return false;
    }

    for (const auto& child : node->Children) {
        if (!IsSubtreeEmpty(child.get())) {
            return false;
        }
    }
    return true;
}

void InsertEntityAtDepth(OC::Node* const node, const entt::entity entity, entt::registry& registry, const int32_t remainingDepth) {
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

    // Compute the octant number [0, 7] the entity bounding box is in.
    // If straddling any of the dividing x, y, z planes, exit.
    for (int32_t i{0}; i < 3; ++i) {
        const float delta{worldSpaceCenter[i] - node->Center[i]};
        if (std::abs(delta) < worldSpaceExtent[i]) {
            straddle = true;
            break;
        }

        if (delta > 0.f) {
            index |= 1 << i;
        }
    }

    // Only push deeper when the entity fits cleanly in one octant AND we still
    // have depth budget left.
    if (!straddle && remainingDepth > 0) {
        if (!node->Children[0]) {
            OC::BuildOctreeNodes(node, node->Center, node->HalfWidth);
        }
        InsertEntityAtDepth(node->Children[index].get(), entity, registry, remainingDepth - 1);
    } else {
        // Straddling or at max depth — store in this node.
        node->Entities.push_back(entity);
        registry.get<OctreeLocation>(entity).Node = node;
    }
}

}

}

void Octree::Init(entt::registry& registry, int32_t maxDepth) {
    _maxDepth = maxDepth;
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

    _rootNode = std::make_unique<OC::Node>(sceneBoundingBox.GetCenter(), sceneBoundingBox.GetExtent()/* * LOOSE_FACTOR*/);

    const auto meshNodeView{registry.view<MeshNodeTag>()};
    for (const auto meshEntity : meshNodeView) {
        // Each mesh that participates in the octree should save the pointer to the nodes it is contained.
        registry.emplace<OctreeLocation>(meshEntity);
        InsertEntity(_rootNode.get(), meshEntity, registry);
    }
}

void Octree::InsertEntity(OC::Node* const node, const entt::entity entity, entt::registry& registry) const {
    OC::InsertEntityAtDepth(node, entity, registry, _maxDepth);
}

void Octree::Update(const entt::entity entity, entt::registry& registry) {
    const auto& localSpaceBoundingBox{registry.get<BoundingBox>(entity).Box};
    const auto& worldTransform{registry.get<WorldTransform>(entity).Value};
    const Box entityWorldSpaceBoundingBox{
        worldTransform.TransformPosition(localSpaceBoundingBox.Min),
        worldTransform.TransformPosition(localSpaceBoundingBox.Max)
    };

    auto* const oldNode{registry.get<OctreeLocation>(entity).Node};
    assert(oldNode);

    // Walk upward from the old node to find an ancestor that fully contains the entity new bounding box.
    auto* octreeNode{oldNode};
    auto octreeNodeBox{Box::FromCenterHalfWidth(octreeNode->Center, octreeNode->HalfWidth)};

    while (!octreeNodeBox.Contains(entityWorldSpaceBoundingBox)) {
        if (!octreeNode->Parent) {
            // Entity has escaped the root, we need to expand.
            break;
        }
        octreeNode = octreeNode->Parent;
        octreeNodeBox = Box::FromCenterHalfWidth(octreeNode->Center, octreeNode->HalfWidth);
    }

    // If the entity is still outside the root, expand the tree.
    if (!octreeNodeBox.Contains(entityWorldSpaceBoundingBox)) {
        // octreeNode is the root here (no parent).
        _expand(entityWorldSpaceBoundingBox);
        // After expansion the root now contains the entity. Insert the entity from the root.
        octreeNode = _rootNode.get();
    }

    // Remove entity from its old node.
    const auto removedElementsNum{oldNode->Entities.remove(entity)};
    assert(removedElementsNum == 1);

    // Insert entity starting from the ancestor found above.
    InsertEntity(octreeNode, entity, registry);

    // Prune any nodes that became empty as a result of the removal.
    _shrink(oldNode);
}

OC::Node* Octree::GetRootNode() const {
    return _rootNode.get();
}

void Octree::_expand(const Box& entityBox) {
    while (!Box::FromCenterHalfWidth(_rootNode->Center, _rootNode->HalfWidth).Contains(entityBox)) {
        const auto entityCenter{entityBox.GetCenter()};

        // Decide which direction to expand toward (one step per axis). The new root is 2x the size of the old root.
        // The old root becomes whichever child is on the side opposite to the entity.
        glm::vec3 newCenter{_rootNode->Center};
        int32_t oldRootChildIndex{0};

        for (int i{0}; i < 3; ++i) {
            if (entityCenter[i] > _rootNode->Center[i]) {
                // The entity is in the positive half. Shift new center positively
                // The old root ends up in the negative-half child (bit = 0).
                newCenter[i] += _rootNode->HalfWidth[i];
            } else {
                // The entity is in the negative half. Shift new center negatively
                // The old root ends up in the positive-half child (bit = 1).
                newCenter[i] -= _rootNode->HalfWidth[i];
                oldRootChildIndex |= (1 << i);
            }
        }

        const auto newHalfWidth{_rootNode->HalfWidth * 2.f};
        auto newRoot{std::make_unique<OC::Node>(newCenter, newHalfWidth)};

        // Build the 7 new children (all except for the old root).
        const glm::vec3 step{newHalfWidth * 0.5f};
        for (int i{0}; i < 8; ++i) {
            if (i == oldRootChildIndex) {
                continue;
            }
            glm::vec3 offset{};
            offset.x = ((i & 1) ? step.x : -step.x);
            offset.y = ((i & 2) ? step.y : -step.y);
            offset.z = ((i & 4) ? step.z : -step.z);
            auto child{std::make_unique<OC::Node>(newCenter + offset, step)};
            child->Parent = newRoot.get();
            newRoot->Children[i] = std::move(child);
        }

        // Attach old root as the appropriate child of the new root.
        _rootNode->Parent = newRoot.get();
        newRoot->Children[oldRootChildIndex] = std::move(_rootNode);

        _rootNode = std::move(newRoot);
    }
}

void Octree::_shrink(OC::Node* node) const {
    while (node && node != _rootNode.get()) {
        // Remove any children whose entire subtree is empty.
        for (auto& child : node->Children) {
            if (child && OC::IsSubtreeEmpty(child.get())) {
                child.reset();
            }
        }

        // If this node itself is now empty and has no remaining children,
        // keep walking upward so the parent can also be pruned.
        const bool selfEmpty{node->Entities.empty()};
        const bool noChildren{std::ranges::all_of(node->Children, [](const auto& c) { return !c; })};

        if (selfEmpty && noChildren) {
            node = node->Parent;
        } else {
            break;
        }
    }
}
