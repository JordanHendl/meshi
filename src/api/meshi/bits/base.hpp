#pragma once
#include <algorithm>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
namespace meshi {
class Component;
class Object {
public:
  Object() {}
  virtual ~Object() = default;

  auto active() -> bool { return m_active; };

  auto set_is_active(bool v) -> void { m_active = v; }

  template <typename T> auto add_subobject() -> T * {
    auto s = std::make_shared<T>();
    m_subobjects.push_back(s);
    return s.get();
  }

  template <typename T> auto add_subobject(typename T::CreateInfo info) -> T * {
    auto s = std::make_shared<T>(info);
    m_subobjects.push_back(s);
    return s.get();
  }

  // Provides filtered vector of all subobjects of that type
  template <typename T> auto subobjects() -> std::vector<T *> {
    auto o = std::vector<T *>();

    std::for_each(m_subobjects.begin(), m_subobjects.end(), [&o](auto i) {
      if (auto ptr = std::dynamic_pointer_cast<T>(i)) {
        o.push_back(ptr.get());
      }
    });

    return o;
  }

  // Provides filtered vector of all subobjects of that type with an optional
  // custom predicate
  template <typename T, typename Predicate>
  auto filter_subobjects(std::vector<T *> *out, Predicate predicate) -> void {
    out->clear();
    std::for_each(m_subobjects.begin(), m_subobjects.end(),
                  [out, &predicate](auto i) {
                    if (auto ptr = std::dynamic_pointer_cast<T>(i)) {
                      if (predicate(ptr.get())) { // Apply the custom predicate
                        out->push_back(ptr.get());
                      }
                    }
                  });
  }

  // Provides filtered vector of all subobjects of that type
  template <typename T> auto filter_subobjects(std::vector<T *> *out) -> void {
    out->clear();
    std::for_each(m_subobjects.begin(), m_subobjects.end(), [out](auto i) {
      if (auto ptr = std::dynamic_pointer_cast<T>(i)) {
        out->push_back(ptr.get());
      }
    });
  }

protected:
  std::vector<std::shared_ptr<Object>> m_subobjects;
  bool m_active = false;
};

class Component : public Object {
public:
  virtual ~Component() = default;
  virtual auto name() -> std::string_view = 0;
};

class Actor : public Object {
public:
  Actor() = default;
  virtual ~Actor() = default;
  virtual auto update(float dt) -> void = 0;
  virtual auto on_activation() -> void {};
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  inline auto get_forward_dir() -> glm::vec3 {

  }

  inline auto local_transform() -> glm::mat4 & { return m_transform; }
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

  inline auto active() -> bool { return m_active; }

  inline auto activate() -> void {
    if (m_active == false) {
      this->on_activation();
    }

    m_active = true;
  }

  inline auto deactivate() -> void { m_active = false; }

protected:
  Actor *m_parent = nullptr;
  bool m_active = false;
  std::unordered_map<std::string, glm::mat4> m_attachment_points;
  glm::mat4 m_transform = {};
};
} // namespace meshi
