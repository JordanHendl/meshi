#pragma once
#include <glm/glm.hpp>
#include <meshi/bits/meshi_c_api.h>
#include <meshi/bits/meshi_loader.hpp>

namespace meshi {

class PhysicsSystem {
public:
  auto
  create_material(PhysicsMaterialCreateInfo &info) -> Handle<PhysicsMaterial> {
    return detail::api().meshi_physx_create_material(m_phys, info);
  }

  void release_material(Handle<PhysicsMaterial> &material) {
    detail::api().meshi_physx_release_material(m_phys, material);
  }

  auto create_rigid_body(RigidBodyCreateInfo &info) -> Handle<RigidBody> {
    return detail::api().meshi_physx_create_rigid_body(m_phys, m_gfx, info);
  }

  void release_rigid_body(Handle<RigidBody> &rigidBody) {
    detail::api().meshi_physx_release_rigid_body(m_phys, rigidBody);
  }

  void apply_force_to_rigid_body(Handle<RigidBody> &rigidBody,
                                 ForceApplyInfo &info) {
    detail::api().meshi_physx_apply_force_to_rigid_body(m_phys, rigidBody, info);
  }

  auto
  get_rigid_body_status(Handle<RigidBody> &rigidBody) -> PhysicsActorStatus & {
    return detail::api().meshi_physx_get_rigid_body_status(m_phys, rigidBody);
  }

private:
  friend class EngineBackend;
  PhysicsSystem() = default;
  PhysicsSystem(RawPhysicsSystem *ptr, RawGraphicsSystem *gfx)
      : m_phys(ptr), m_gfx(gfx) {}
  ~PhysicsSystem() = default;

  RawGraphicsSystem *m_gfx;
  RawPhysicsSystem *m_phys;
};

} // namespace meshi
