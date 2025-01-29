#pragma once
#include "meshi/engine.hpp"
#include "meshi/bits/component.hpp"
#include "meshi/meshi_c_api.h"
#include <meshi/bits/base.hpp>
#include <meshi/bits/util/slice.hpp>
namespace meshi {
class RenderableComponent : public ActorComponent {
public:
  RenderableComponent() {}
  virtual ~RenderableComponent() = default;
};

class MeshComponent : public RenderableComponent {
public:
  using CreateInfo = MeshComponentInfo;
  MeshComponent(MeshComponentInfo info) {
    auto backend = engine()->backend();
    m_handle = meshi_register_mesh_object(backend, info);
    m_name = info.mesh;
  }

  virtual auto name() -> std::string_view { return m_name; }

  virtual auto update(float dt) -> void {
    meshi_set_mesh_object_transform(*engine()->backend(), m_handle,
                                    m_transform);
  }

  virtual ~MeshComponent() = default;

protected:
  std::string m_name;
  Handle<FFIMeshComponent> m_handle;
  glm::mat4 m_transform;
};

struct CubeMeshComponentInfo {
  const char *material = "";
};
class CubeMeshComponent : public MeshComponent {
public:
  using CreatEInfo = CubeMeshComponentInfo;
  CubeMeshComponent(CubeMeshComponentInfo info = {})
      : MeshComponent(MeshComponentInfo{
            .mesh = "MESHI.CUBE",
            .material = info.material,
        }) {}
};
} // namespace meshi
