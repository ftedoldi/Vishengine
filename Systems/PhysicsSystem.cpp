#include "PhysicsSystem.h"
#include "Components/MeshObject.h"

static constexpr float STEP{0.0333f};
static constexpr glm::vec3 GRAVITY{0.f, -9.81f, 0.f};
static constexpr float DAMPING{0.6f};
static constexpr uint32_t SOLVER_ITERATIONS{2};

PhysicsSystem::PhysicsSystem(entt::registry& registry) : _registry{registry} {
}

void PhysicsSystem::Update() {
    auto view{_registry.view<MeshObject>()};

    view.each([this](const MeshObject& meshObject) {
        for(const auto& mesh : meshObject.Meshes) {
            #pragma omp parallel for
            for (size_t i{0}; i < mesh->PointsMasses.Positions.size() - 1; ++i) {
                mesh->PointsMasses.OldPositions[i] = mesh->PointsMasses.Positions[i];
                mesh->PointsMasses.Velocities[i] += STEP * GRAVITY;
                mesh->PointsMasses.Velocities[i] *= DAMPING;
                mesh->PointsMasses.Positions[i] += STEP * mesh->PointsMasses.Velocities[i];
            }

            // 2. Solve constraints
            for(uint32_t i{0}; i < SOLVER_ITERATIONS; ++i) {
                #pragma omp parallel for
                for(auto& constraint : mesh->DistanceConstraints) {
                    constraint.Project();
                }
            }

            // 3. Update velocities
            #pragma omp parallel for
            for(size_t i = 0; i < mesh->PointsMasses.Positions.size(); ++i) {
                mesh->PointsMasses.Velocities[i] = (mesh->PointsMasses.Positions[i] - mesh->PointsMasses.OldPositions[i]) / STEP;
            }
        }
    });
}
