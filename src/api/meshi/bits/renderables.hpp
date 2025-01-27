#pragma once
#include "meshi/engine.hpp"
#include "meshi/meshi_c_api.h"
#include <meshi/bits/base.hpp>
#include <meshi/bits/util/slice.hpp>
namespace meshi {
class Renderable : public Component {
public:
  Renderable() {}

  virtual ~Renderable() = default;
};

class MeshObject : public Renderable {
public:
  using CreateInfo = MeshObjectInfo;
  MeshObject(MeshObjectInfo info) {
    auto backend = engine()->backend();
    m_handle = meshi_register_mesh_object(backend, info);
    m_name = info.mesh;
  }

  virtual auto name() -> std::string_view { return m_name; }

  virtual auto set_transform(glm::mat4 &transform) -> void {
    m_transform = transform;
  }

  virtual auto update(float dt) -> void {
    meshi_set_mesh_object_transform(*engine()->backend(), m_handle,
                                    m_transform);
  }

  virtual ~MeshObject() = default;

protected:
  std::string m_name;
  Handle<FFIMeshObject> m_handle;
  glm::mat4 m_transform;
};

struct CubeMeshObjectInfo {
  const char *material = "";
};
class CubeMeshObject : public MeshObject {
public:
  using CreatEInfo = CubeMeshObjectInfo;
  CubeMeshObject(CubeMeshObjectInfo info = {})
      : MeshObject(MeshObjectInfo{
            .mesh = "MESHI.CUBE",
            .material = info.material,
        }) {}
};

class Entity : public Renderable {
public:
};
} // namespace meshi
