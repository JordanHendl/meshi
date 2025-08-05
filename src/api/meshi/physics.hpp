#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdint>
#include <meshi/bits/meshi.h>
#include <meshi/bits/meshi_loader.hpp>
#include <meshi/bits/util/handle.hpp>
#include <meshi/c_api_types.hpp>

// C API type definitions are provided in c_api_types.hpp

namespace meshi {
struct PhysicsMaterial {};
struct RigidBody {};

struct PhysicsMaterialCreateInfo {
  float dynamic_friction = 5.0f;
};

struct ForceApplyInfo {
  glm::vec3 amount{0.f,0.f,0.f};
};

struct RigidBodyCreateInfo {
  Handle<PhysicsMaterial> material{};
  glm::vec3 initial_position{0.f,0.f,0.f};
  glm::vec3 initial_velocity{0.f,0.f,0.f};
  glm::quat initial_rotation{1.f,0.f,0.f,0.f};
  std::uint32_t has_gravity = 0;
};

struct PhysicsActorStatus {
  glm::mat4 transform{1.0f};
};

class PhysicsSystem {
public:
  auto create_material(PhysicsMaterialCreateInfo &info) -> Handle<PhysicsMaterial> {
    ::MaterialInfo raw{info.dynamic_friction};
    ::Handle h = detail::api().meshi_physx_create_material(m_phys, &raw);
    return Handle<PhysicsMaterial>{static_cast<uint16_t>(h.index), static_cast<uint16_t>(h.generation)};
  }

  void release_material(Handle<PhysicsMaterial> &material) {
    ::Handle h{material.slot, material.generation};
    detail::api().meshi_physx_release_material(m_phys, &h);
  }

  auto create_rigid_body(RigidBodyCreateInfo &info) -> Handle<RigidBody> {
    ::RigidBodyInfo raw{};
    raw.material = {info.material.slot, info.material.generation};
    raw.initial_position = {info.initial_position.x, info.initial_position.y, info.initial_position.z};
    raw.initial_velocity = {info.initial_velocity.x, info.initial_velocity.y, info.initial_velocity.z};
    raw.initial_rotation = {info.initial_rotation.x, info.initial_rotation.y, info.initial_rotation.z, info.initial_rotation.w};
    raw.has_gravity = info.has_gravity;
    raw.collision_shape = detail::api().meshi_physx_collision_shape_sphere(0.5f);
    ::Handle h = detail::api().meshi_physx_create_rigid_body(m_phys, &raw);
    return Handle<RigidBody>{static_cast<uint16_t>(h.index), static_cast<uint16_t>(h.generation)};
  }

  void release_rigid_body(Handle<RigidBody> &rigidBody) {
    ::Handle h{rigidBody.slot, rigidBody.generation};
    detail::api().meshi_physx_release_rigid_body(m_phys, &h);
  }

  void apply_force_to_rigid_body(Handle<RigidBody> &rigidBody, ForceApplyInfo &info) {
    ::Handle h{rigidBody.slot, rigidBody.generation};
    ::ForceApplyInfo raw{{info.amount.x, info.amount.y, info.amount.z}};
    detail::api().meshi_physx_apply_force_to_rigid_body(m_phys, &h, &raw);
  }

  auto get_rigid_body_status(Handle<RigidBody> &rigidBody) -> PhysicsActorStatus {
    ::Handle h{rigidBody.slot, rigidBody.generation};
    ::ActorStatus raw{};
    detail::api().meshi_physx_get_rigid_body_status(m_phys, &h, &raw);
    glm::quat rot(raw.rotation.w, raw.rotation.x, raw.rotation.y, raw.rotation.z);
    glm::mat4 transform = glm::translate(glm::mat4(1.0f),
                              glm::vec3(raw.position.x, raw.position.y, raw.position.z)) *
                          glm::toMat4(rot);
    return PhysicsActorStatus{transform};
  }

private:
  friend class EngineBackend;
  PhysicsSystem() = default;
  explicit PhysicsSystem(PhysicsSimulation *ptr) : m_phys(ptr) {}
  ~PhysicsSystem() = default;

  PhysicsSimulation *m_phys;
};

} // namespace meshi
