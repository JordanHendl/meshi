#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstring>
#include <meshi/meshi.h>
#include "meshi/types.hpp"

namespace meshi {

class GraphicsSystem {
public:
  auto create_renderable(const gfx::RenderableCreateInfo &info)
      -> Handle<gfx::Renderable> {
    MeshiFFIMeshObjectInfo ffi_info{
        info.mesh,
        info.material,
        to_meshi_mat4(info.transform),
    };
    return meshi_gfx_create_renderable(m_gfx, &ffi_info);
  }

  auto create_directional_light(const gfx::DirectionalLightInfo &info)
      -> Handle<gfx::DirectionalLight> {
    MeshiDirectionalLightInfo ffi{};
    ffi.direction = {info.direction.x, info.direction.y, info.direction.z,
                     info.direction.w};
    ffi.color = {info.color.x, info.color.y, info.color.z, info.color.w};
    ffi.intensity = info.intensity;
    return meshi_gfx_create_directional_light(m_gfx, &ffi);
  }

  void set_transform(Handle<gfx::Renderable> &renderable,
                     const glm::mat4 &transform) {
    MeshiMat4 t = to_meshi_mat4(transform);
    meshi_gfx_set_renderable_transform(m_gfx, renderable, &t);
  }

  void set_camera(const glm::mat4 &view_matrix) {
    MeshiMat4 t = to_meshi_mat4(view_matrix);
    meshi_gfx_set_camera(m_gfx, &t);
  }

  void set_projection(const glm::mat4 &projection_matrix) {
    MeshiMat4 t = to_meshi_mat4(projection_matrix);
    meshi_gfx_set_projection(m_gfx, &t);
  }
  
  inline auto capture_mouse(bool value) -> void {
    meshi_gfx_capture_mouse(m_gfx, static_cast<int>(value));
  }
private:
  friend class EngineBackend;
  GraphicsSystem() = default;
  explicit GraphicsSystem(RawGraphicsSystem *ptr) : m_gfx(ptr) {}
  ~GraphicsSystem() = default;

  static MeshiMat4 to_meshi_mat4(const glm::mat4 &m) {
    MeshiMat4 out{};
    std::memcpy(out.m, glm::value_ptr(m), sizeof(MeshiMat4));
    return out;
  }

  RawGraphicsSystem *m_gfx{};
};

} // namespace meshi
