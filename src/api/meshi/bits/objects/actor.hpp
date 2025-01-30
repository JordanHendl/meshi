#pragma once
#include <meshi/bits/objects/base.hpp>
#include <meshi/bits/components/actor_component.hpp>
namespace meshi {

// Object type
// Represents any object in the scene.
class Actor : public Object {
public:
  Actor() {
    m_root_component = std::make_shared<ActorComponent>();
    m_root_component->m_actor = this;
  };

  virtual ~Actor() = default;
  virtual auto update(float dt) -> void {
    if (m_root_component) {
      m_root_component->update(dt);
    }
  };

  virtual auto activate() -> void {
    Object::activate();
    if (m_root_component) {
      m_root_component->activate();
    }
  }

  virtual auto deactivate() -> void {
    Object::deactivate();
    if (m_root_component) {
      m_root_component->deactivate();
    }
  }
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  
  inline auto up() -> glm::vec3 {return m_root_component->up();}
  inline auto right() -> glm::vec3 {return m_root_component->right();}
  inline auto front() -> glm::vec3 {return m_root_component->front();}
  inline auto local_transform() -> glm::mat4  { return m_root_component->world_transform(); }
  inline auto world_transform() -> glm::mat4 {
    if (m_parent) {
      return m_parent->world_transform() * local_transform();
    }

    return local_transform();
  }
  inline auto set_transform(glm::mat4 &transform) -> void {
    m_transform = transform;
  }

  inline auto detach_owner() -> void { set_owner(nullptr); }

  inline auto set_owner(Actor *parent) -> void { m_parent = parent; }

  inline auto add_attachment_point(std::string name,
                                   glm::mat4 &transformation) -> void {
    m_attachment_points.insert({name, transformation});
  }

  inline auto remove_attachment_point(std::string_view name) -> bool {
    auto iter = m_attachment_points.find(std::string(name));
    if (iter != m_attachment_points.end()) {
      m_attachment_points.erase(iter);
      return true;
    }
    return false;
  }

  inline auto attachment_transformation(std::string_view name) -> glm::mat4 & {
    static auto d = glm::mat4(1.0f);
    auto iter = m_attachment_points.find(std::string(name));
    if (iter != m_attachment_points.end()) {
      return iter->second;
    }

    return d;
  }

  inline auto root_component() -> ActorComponent * { return m_root_component.get(); }

protected:
  std::shared_ptr<ActorComponent> m_root_component = nullptr;
  Actor *m_parent = nullptr;
  std::unordered_map<std::string, glm::mat4> m_attachment_points;
  glm::mat4 m_transform = {};
};
}
