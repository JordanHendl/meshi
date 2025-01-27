#pragma once
#include <algorithm>
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

  template <typename T> auto add_component() -> T * {
    auto s = std::make_shared<T>();
    m_components.push_back(s);
    return s.get();
  }

  template <typename T> auto add_component(typename T::CreateInfo info) -> T * {
    auto s = std::make_shared<T>(info);
    m_components.push_back(s);
    return s.get();
  }

  // Provides filtered vector of all components of that type
  template <typename T> auto components() -> std::vector<T *> {
    auto o = std::vector<T *>();

    std::for_each(m_components.begin(), m_components.end(), [&o](auto i) {
      if (auto ptr = std::dynamic_pointer_cast<T>(i)) {
        o.push_back(ptr.get());
      }
    });

    return o;
  }

  // Provides filtered vector of all components of that type
  template <typename T>
  auto filter_components(std::vector<T*> *out) -> void {
    out->clear();
    std::for_each(m_components.begin(), m_components.end(), [out](auto i) {
      if (auto ptr = std::dynamic_pointer_cast<T>(i)) {
        out->push_back(ptr.get());
      }
    });
  }

protected:
  std::vector<std::shared_ptr<Object>> m_components;
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
};

class Denizen : public Object {
public:
  Denizen() = default;
  virtual ~Denizen() = default;
};
} // namespace meshi
