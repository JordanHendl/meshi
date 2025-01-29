#pragma once
#include <meshi/bits/base.hpp>
namespace meshi {
class Actor;

class ActorComponent : public Component {
public:
  inline auto update(float dt) -> void override {}
  inline auto local_transform() -> glm::mat4 & { return m_transform; }
  inline auto world_transform() -> glm::mat4 {
    if (auto actor = this->get_actor()) {
      if(m_parent)
        return actor->world_transform() * local_transform();
    }

    
    return local_transform();
  }
  inline auto set_transform(glm::mat4 &transform) -> void {
    m_transform = transform;
  }

private:
  glm::mat4 m_transform;
};

} // namespace meshi
