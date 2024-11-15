#include "PhysicsSystem.h"
#include "Components/MeshObject.h"
#include "Components/Position.h"

static constexpr float STEP{0.0333f};
static constexpr glm::vec3 GRAVITY{0.f, -9.81f, 0.f};
static constexpr float DAMPING{0.6f};
static constexpr uint32_t SOLVER_ITERATIONS{8};

PhysicsSystem::PhysicsSystem(entt::registry& registry) : _registry{registry} {
}

void PhysicsSystem::Update() {
    auto view{_registry.view<MeshObject, Position>()};

    view.each([](const MeshObject& meshObject, const Position worldPosition) {
        for(const auto& mesh : meshObject.Meshes) {
            // Update mesh data
            #pragma omp parallel for
            for (size_t i{0}; i < mesh->ParticlesData.Positions.size(); ++i) {
                mesh->ParticlesData.OldPositions[i] = mesh->ParticlesData.Positions[i];
                mesh->ParticlesData.Velocities[i] += STEP * mesh->ParticlesData.InverseMasses[i] * GRAVITY;
                mesh->ParticlesData.Velocities[i] *= DAMPING;
                mesh->ParticlesData.Positions[i] += STEP * mesh->ParticlesData.Velocities[i];

                if(mesh->ParticlesData.Positions[i].y + worldPosition.Vector.y < 0) {
                    mesh->ParticlesData.Positions[i].y = -worldPosition.Vector.y;
                    mesh->ParticlesData.Velocities[i].y = 0.f;
                }
            }

            // Solve constraints
            for(uint32_t i{0}; i < SOLVER_ITERATIONS; ++i) {
                #pragma omp parallel for
                for(auto constraint : mesh->DistanceConstraints) {
                    constraint.Project();
                }
            }

            // Update velocities
            #pragma omp parallel for
            for(size_t i = 0; i < mesh->ParticlesData.Positions.size(); ++i) {
                mesh->ParticlesData.Velocities[i] = (mesh->ParticlesData.Positions[i] - mesh->ParticlesData.OldPositions[i]) / STEP;
            }
        }
    });
}
