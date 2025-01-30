#pragma once
#include <algorithm>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
namespace meshi {
class Component;

// Basis of all Objects. 
// Can contain sub-objects.
// Can be activated/deactivated.
class Object {
public:
  //////////////////////////////////////////////////////
  ////VIRTUAL FUNCITONS/////////////////////////////////
  //////////////////////////////////////////////////////

  Object() {}
  virtual ~Object() = default;
  virtual auto on_activation() -> void {};
  virtual auto on_deactivation() -> void {};

  //////////////////////////////////////////////////////
  ////NON-VIRTUAL FUNCTIONS/////////////////////////////
  //////////////////////////////////////////////////////

  auto active() -> bool { return m_active; };
  virtual auto activate() -> void {
    if (!m_active)
      on_activation();
    m_active = true;
  }

  virtual auto deactivate() -> void {
    if (m_active)
      on_deactivation();
    m_active = false;
  }

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

  template <typename T> inline auto is_type() -> bool {
    return (std::dynamic_pointer_cast<T>(this));
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
} // namespace meshi
