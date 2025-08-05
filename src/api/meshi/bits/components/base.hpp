#pragma once
#include "meshi/bits/objects/base.hpp"
#include <algorithm>
#include <iterator>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace meshi {
class Actor;
class Component : public Object {
public:

  //////////////////////////////////////////////////////
  ////VIRTUAL FUNCITONS/////////////////////////////////
  //////////////////////////////////////////////////////

  virtual ~Component() = default;
  virtual auto update(float dt) -> void {
    for (auto *child : m_children) {
      child->update(dt);
    }
  };

  virtual auto activate() -> void {
    Object::activate();
    for (auto *child : m_children) {
      child->activate();
    }
  }

  virtual auto deactivate() -> void {
    Object::deactivate();
    for (auto *child : m_children) {
      child->deactivate();
    }
  }

  //////////////////////////////////////////////////////
  ////NON-VIRTUAL FUNCTIONS/////////////////////////////
  //////////////////////////////////////////////////////

  template<typename T>
  inline auto as_type() -> T* {
    return dynamic_cast<T*>(this);
  }

  template<typename T>
  inline auto is_type() -> bool {
    return as_type<T>();
  }

  // Detaches from any parent component.
  inline auto detach() -> void {
    if (m_parent) {
      auto iter =
          std::find_if(std::begin(m_parent->m_children), std::end(m_children),
                       [this](auto c) { return c == this; });
      if (iter != std::end(m_parent->m_children))
        m_parent->m_children.erase(iter);

      deactivate();
    }
  }

  inline auto attach_to(Component *parent) -> void {
    m_parent = parent;
    parent->m_children.push_back(this);
  }

  // Traverses up the component chain to retrieve the parent actor, if there is
  // one.
  inline auto get_actor() -> Actor * {
    auto ptr = this;
    while (ptr->m_parent != nullptr) {
      ptr = ptr->m_parent;
    }

    return ptr->m_actor;
  }

  // Traverses up the component chain to retrieve the root component, if there
  // is one. Null otherwise.
  inline auto get_root_component() -> Component * {
    auto ptr = this;
    while (ptr->m_parent != nullptr) {
      ptr = ptr->m_parent;
    }

    if (ptr->m_actor != nullptr) {
      return ptr;
    }
    return nullptr;
  }

protected:
  Actor *m_actor = nullptr;
  Component *m_parent = nullptr;
  std::vector<Component *> m_children;
  bool m_active = false;
};
} // namespace meshi
