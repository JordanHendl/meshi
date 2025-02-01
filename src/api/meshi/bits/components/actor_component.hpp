#pragma once
#include <meshi/bits/components/base.hpp>
namespace meshi {
class ActorComponent : public Component {
public:
  //////////////////////////////////////////////////////
  ////VIRTUAL FUNCITONS/////////////////////////////////
  //////////////////////////////////////////////////////

  //////////////////////////////////////////////////////
  ////NON-VIRTUAL FUNCTIONS/////////////////////////////
  //////////////////////////////////////////////////////

  inline auto update(float dt) -> void override {}

  inline auto front() -> glm::vec3 { return m_front; }

  inline auto right() -> glm::vec3 { return m_right; }

  inline auto up() -> glm::vec3 { return m_up; }
  inline auto local_transform() -> glm::mat4 & { return m_transform; }
  inline auto world_transform() -> glm::mat4 {
    if (auto ptr = m_parent->as_type<ActorComponent>()) {
      return ptr->world_transform() * local_transform();
    }

    return local_transform();
  }

  inline auto set_transform(glm::mat4 &transform) -> void {
    m_right = glm::vec3(transform[0]);
    m_up = glm::vec3(transform[1]);   
    m_front = glm::vec3(transform[2]);
    m_transform = transform;
  }

private:
  friend class Actor;
  glm::mat4 m_transform = glm::mat4(1.0);
  ;
  glm::vec3 m_front = {0.0, 0.0, 1.0};
  glm::vec3 m_right = {1.0, 0.0, 0.0};
  glm::vec3 m_up = {0.0, 1.0, 0.0};
};
} // namespace meshi
