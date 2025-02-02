#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <meshi/bits/meshi_c_api.h>

namespace meshi {

class GraphicsSystem {
public:
  auto create_renderable(gfx::RenderableCreateInfo &info)
      -> Handle<gfx::Renderable> {
    return meshi_gfx_create_renderable(m_gfx, info);
  }

  void set_transform(Handle<gfx::Renderable> &renderable,
                     glm::mat4 &transform) {
    meshi_gfx_set_transform(m_gfx, renderable, (transform));
  }

  void set_camera(glm::mat4 &view_matrix) {
    meshi_gfx_set_camera(m_gfx, (view_matrix));
  }

  void set_projection(glm::mat4 &projection_matrix) {
    meshi_gfx_set_projection(m_gfx, (projection_matrix));
  }

private:
  friend class EngineBackend;
  GraphicsSystem() = default;
  GraphicsSystem(RawGraphicsSystem *ptr) : m_gfx(ptr) {}
  ~GraphicsSystem() = default;

  RawGraphicsSystem *m_gfx;
};

} // namespace meshi
