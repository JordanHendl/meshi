#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <meshi/bits/meshi_c_api.h>
#include <meshi/bits/meshi_loader.hpp>

namespace meshi {

class GraphicsSystem {
public:
  auto create_renderable(const gfx::RenderableCreateInfo &info)
      -> Handle<gfx::Renderable> {
    return detail::api().meshi_gfx_create_renderable(m_gfx, info);
  }
  
  auto create_directional_light(const gfx::DirectionalLightInfo& info) -> Handle<gfx::DirectionalLight>{
    return detail::api().meshi_gfx_create_directional_light(m_gfx, info);
  }

  void set_transform(Handle<gfx::Renderable> &renderable,
                     glm::mat4 &transform) {
    detail::api().meshi_gfx_set_renderable_transform(m_gfx, renderable, (transform));
  }

  void set_camera(glm::mat4 &view_matrix) {
    detail::api().meshi_gfx_set_camera(m_gfx, (view_matrix));
  }

  void set_projection(glm::mat4 &projection_matrix) {
    detail::api().meshi_gfx_set_projection(m_gfx, (projection_matrix));
  }
  
  inline auto capture_mouse(bool value) -> void {
    detail::api().meshi_gfx_capture_mouse(m_gfx, static_cast<int>(value));
  }
private:
  friend class EngineBackend;
  GraphicsSystem() = default;
  GraphicsSystem(RawGraphicsSystem *ptr) : m_gfx(ptr) {}
  ~GraphicsSystem() = default;

  RawGraphicsSystem *m_gfx;
};

} // namespace meshi
