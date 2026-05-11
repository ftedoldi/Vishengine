#include "Octree.h"

#include "Box.h"
#include "Components/BoundingBox.h"
#include "Components/OctreeLocation.h"
#include "Components/Transforms/WorldTransform.h"
#include "Components/WorldBoundingBox.h"
#include "Coordinates/CoordinateUtils.h"
#include "Frustum.h"

#include "entt/entity/registry.hpp"
#include "glm/common.hpp"

#include <limits>
#include <queue>
#include <ranges>
#include <vector>

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

void CollectEntitiesInFrustum(const Node* const node, const Frustum& frustum,
                               std::vector<entt::entity>& result, const bool fullyInside) {
    if (!node) return;

    bool childrenFullyInside = fullyInside;
    if (!fullyInside) {
        const Box nodeBox{Box::FromCenterHalfWidth(node->Center, node->HalfWidth)};
        const auto classification{FrustumUtils::ClassifyAABB(nodeBox, frustum)};
        if (classification == FrustumUtils::Intersection::Outside) return;
        childrenFullyInside = (classification == FrustumUtils::Intersection::Inside);
    }

    for (const auto entity : node->Entities) {
        result.push_back(entity);
    }

    for (const auto& child : node->Children) {
        CollectEntitiesInFrustum(child.get(), frustum, result, childrenFullyInside);
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



void InsertEntityAtDepth(Node* const node, const entt::entity entity, entt::registry& registry, const int32_t remainingDepth) {
    assert(node);
    const Box worldSpaceBox{
        registry.all_of<WorldBoundingBox>(entity)
            ? registry.get<WorldBoundingBox>(entity).Box
            : CoordUtils::ComputeWorldSpaceBox(entity, registry)
    };
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
            BuildOctreeNodes(node, node->Center, node->HalfWidth);
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

void Octree::Init(entt::registry& registry, const int32_t maxDepth) {
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

    const auto pickableEntitiesView{registry.view<BoundingBox, WorldTransform>()};
    for (const auto pickableEntity : pickableEntitiesView) {
        registry.emplace<OctreeLocation>(pickableEntity);
        InsertEntity(_rootNode.get(), pickableEntity, registry);
    }
}

void Octree::InsertEntity(OC::Node* const node, const entt::entity entity, entt::registry& registry) const {
    OC::InsertEntityAtDepth(node, entity, registry, _maxDepth);
}

void Octree::Update(const entt::entity entity, entt::registry& registry) {
    const Box entityWorldSpaceBoundingBox{
        registry.all_of<WorldBoundingBox>(entity)
            ? registry.get<WorldBoundingBox>(entity).Box
            : CoordUtils::ComputeWorldSpaceBox(entity, registry)
    };

    auto* const oldNode{registry.get<OctreeLocation>(entity).Node};
    assert(oldNode);

    // Walk upward from the old node to find an ancestor that fully contains the entity new bounding box.
    auto* octreeNode{oldNode};
    auto octreeNodeBox{Box::FromCenterHalfWidth(octreeNode->Center, octreeNode->HalfWidth)};

    bool expandOctree{};
    while (!octreeNodeBox.Contains(entityWorldSpaceBoundingBox)) {
        if (!octreeNode->Parent) {
            // Entity has escaped the root, we need to expand.
            expandOctree = true;
            break;
        }
        octreeNode = octreeNode->Parent;
        octreeNodeBox = Box::FromCenterHalfWidth(octreeNode->Center, octreeNode->HalfWidth);
    }

    // If the entity is still outside the root, expand the tree.
    if (expandOctree) {
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

    // Shrink the octree if only one of the child nodes of the root node exists.
    _shrink();
}

std::optional<RaycastHit> Octree::Raycast(const Ray& ray, entt::registry& registry) const {
    if (!_rootNode) {
        return std::nullopt;
    }

    float rootDistance{};
    if (!Raycaster::IntersectBox(ray, Box::FromCenterHalfWidth(_rootNode->Center, _rootNode->HalfWidth), rootDistance)) {
        return std::nullopt;
    }

    struct NodeRayHit {
        const OC::Node* Node{};
        float Distance{};
    };

    const auto compareClosestFirst{[](const NodeRayHit& lhs, const NodeRayHit& rhs) {
        return lhs.Distance > rhs.Distance;
    }};

    std::priority_queue<NodeRayHit, std::vector<NodeRayHit>, decltype(compareClosestFirst)> nodesToVisit{compareClosestFirst};
    nodesToVisit.push(NodeRayHit{_rootNode.get(), rootDistance});

    RaycastHit closestHit{};
    float closestDistance{ray.MaxDistance};
    bool foundHit{};

    while (!nodesToVisit.empty()) {
        const auto [node, nodeEntryDistance]{nodesToVisit.top()};
        nodesToVisit.pop();

        if (!node || nodeEntryDistance > closestDistance) {
            continue;
        }

        for (const auto entity : node->Entities) {
            if (!registry.valid(entity) || !registry.all_of<BoundingBox, WorldTransform>(entity)) {
                continue;
            }

            const Box entityBox{CoordUtils::ComputeWorldSpaceBox(entity, registry)};
            float entityDistance{};
            if (!Raycaster::IntersectBox(ray, entityBox, entityDistance) || entityDistance > closestDistance) {
                continue;
            }

            closestDistance = entityDistance;
            closestHit = RaycastHit{entity, entityDistance, ray.Origin + ray.Direction * entityDistance};
            foundHit = true;
        }

        for (const auto& child : node->Children) {
            if (!child) {
                continue;
            }

            float childEntryDistance{};
            if (Raycaster::IntersectBox(ray, Box::FromCenterHalfWidth(child->Center, child->HalfWidth), childEntryDistance) &&
                childEntryDistance <= closestDistance) {
                nodesToVisit.push(NodeRayHit{child.get(), childEntryDistance});
            }
        }
    }

    if (!foundHit) {
        return std::nullopt;
    }

    return closestHit;
}

std::vector<entt::entity> Octree::QueryFrustum(const Frustum& frustum) const {
    std::vector<entt::entity> result{};
    if (_rootNode) {
        OC::CollectEntitiesInFrustum(_rootNode.get(), frustum, result, false);
    }
    return result;
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

void Octree::_shrink() {
    // TODO: can be optimized
    uint32_t numOfRootChildrenWithEntities{};

    // No need to shrink if the root node has no children.
    if (!_rootNode->Entities.empty()) {
        return;
    }

    for (const auto& child : _rootNode->Children) {
        if (child && !OC::IsSubtreeEmpty(child.get())) {
            numOfRootChildrenWithEntities++;
        }
    }

    if (numOfRootChildrenWithEntities == 1) {
        auto childWithEntitiesIterator{std::ranges::find_if(_rootNode->Children, [](const auto& child) {
            return !OC::IsSubtreeEmpty(child.get());
        })};

        auto childWithEntities = std::move(*childWithEntitiesIterator);
        childWithEntities->Parent = nullptr;

        _rootNode = std::move(childWithEntities);
    }
}
