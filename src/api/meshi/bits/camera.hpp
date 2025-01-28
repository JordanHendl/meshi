#pragma once
#include "glm/ext/matrix_clip_space.hpp"
#include <meshi/bits/base.hpp>
namespace meshi { 
class Camera;
namespace detail {
static Camera *world_camera = nullptr;
}
class Camera : public Actor {
public:
  Camera() {
    constexpr auto fov = 90.0;
    constexpr auto aspect = 16.0/9.0;
    constexpr auto near = 0.01;
    constexpr auto far = 2000.0;
    m_projection = glm::perspective(fov, aspect, near, far);
  };
  virtual ~Camera() {
    if(detail::world_camera == this) detail::world_camera = nullptr;
  };
  inline auto view_matrix() -> glm::mat4 {
    return glm::inverse(this->world_transform());
  }

  virtual auto update(float dt) -> void override {}
  inline auto projection() -> glm::mat4 { return this->m_projection; }

  inline auto apply_to_world() -> void {
    detail::world_camera = this;
  }

private:
  glm::mat4 m_projection;
};
} // namespace meshi
