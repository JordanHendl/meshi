#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdint>
#include <cstring>
#include <meshi/bits/meshi.h>
#include <meshi/bits/meshi_loader.hpp>
#include <meshi/bits/util/handle.hpp>
#include <meshi/c_api_types.hpp>

// Basic C API type definitions are provided in c_api_types.hpp

namespace meshi {
namespace gfx {
struct Renderable {};
struct DirectionalLight {};

struct RenderableCreateInfo {
  const char* mesh = "";
  const char* material = "";
  glm::mat4 transform = glm::mat4(1.0f);
};

struct DirectionalLightInfo {
  glm::vec4 direction{0.f,0.f,0.f,0.f};
  glm::vec4 color{0.f,0.f,0.f,0.f};
  float intensity{0.f};
};
} // namespace gfx

class GraphicsSystem {
public:
  auto create_renderable(const gfx::RenderableCreateInfo &info) -> Handle<gfx::Renderable> {
    ::FFIMeshObjectInfo raw{};
    raw.mesh = info.mesh;
    raw.material = info.material;
    std::memcpy(&raw.transform, glm::value_ptr(info.transform), sizeof(Mat4));
    ::Handle h = detail::api().meshi_gfx_create_renderable(m_gfx, &raw);
    return Handle<gfx::Renderable>{static_cast<uint16_t>(h.index), static_cast<uint16_t>(h.generation)};
  }

  auto create_directional_light(const gfx::DirectionalLightInfo &info) -> Handle<gfx::DirectionalLight> {
    ::DirectionalLightInfo raw{};
    std::memcpy(&raw.direction, &info.direction, sizeof(glm::vec4));
    std::memcpy(&raw.color, &info.color, sizeof(glm::vec4));
    raw.intensity = info.intensity;
    ::Handle h = detail::api().meshi_gfx_create_directional_light(m_gfx, &raw);
    return Handle<gfx::DirectionalLight>{static_cast<uint16_t>(h.index), static_cast<uint16_t>(h.generation)};
  }

  void set_transform(Handle<gfx::Renderable> &renderable, glm::mat4 &transform) {
    ::Handle raw{renderable.slot, renderable.generation};
    detail::api().meshi_gfx_set_renderable_transform(m_gfx, raw,
        reinterpret_cast<const Mat4*>(&transform));
  }

  void set_camera(glm::mat4 &view_matrix) {
    detail::api().meshi_gfx_set_camera(m_gfx,
        reinterpret_cast<const Mat4*>(&view_matrix));
  }

  void set_projection(glm::mat4 &projection_matrix) {
    detail::api().meshi_gfx_set_projection(m_gfx,
        reinterpret_cast<const Mat4*>(&projection_matrix));
  }

  inline auto capture_mouse(bool value) -> void {
    detail::api().meshi_gfx_capture_mouse(m_gfx, static_cast<int32_t>(value));
  }

private:
  friend class EngineBackend;
  GraphicsSystem() = default;
  explicit GraphicsSystem(RenderEngine *ptr) : m_gfx(ptr) {}
  ~GraphicsSystem() = default;

  RenderEngine *m_gfx;
};

} // namespace meshi
