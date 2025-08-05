#pragma once
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/ext/matrix_clip_space.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <meshi/bits/components/actor_component.hpp>
namespace meshi { 
class CameraComponent;
namespace detail {
static CameraComponent *world_camera = nullptr;
}
class CameraComponent : public ActorComponent {
public:
  CameraComponent() {
    constexpr auto fov = 20.0;
    constexpr auto aspect = 16.0/9.0;
    constexpr auto near = 0.1;
    constexpr auto far = 200000.0;
    m_projection = glm::perspective(glm::radians(fov), aspect, near, far);
  };
  virtual ~CameraComponent() {
    if(detail::world_camera == this) detail::world_camera = nullptr;
  };
  inline auto view_matrix() -> glm::mat4 {
    return (glm::inverse(this->world_transform()));
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
