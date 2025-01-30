#pragma once
#include <meshi/bits/components/mesh_component.hpp>
#include <meshi/bits/util/slice.hpp>
namespace meshi {
struct CubeMeshComponentInfo {
  const char *material = "";
  RigidBodyCreateInfo rigid_body_info;
};
class CubeMeshComponent : public MeshComponent {
public:
  using CreateInfo = CubeMeshComponentInfo;
  CubeMeshComponent(CubeMeshComponentInfo info = {})
      : MeshComponent({gfx::RenderableCreateInfo{
                           .mesh = "MESHI.CUBE",
                           .material = info.material,
                       },
                       info.rigid_body_info}) {}
};
} // namespace meshi
