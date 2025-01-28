#pragma once
#include "meshi/meshi_c_api.h"
#include <algorithm>
#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <meshi/bits/event.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace meshi {

// Struct representing an action
struct Action {
  std::string type; // Action type as a string
  uint32_t timestamp;
};

// Define a filter function type for actions
using ActionFilter = std::function<bool(const Event &, Action &)>;

// Define a callback function type for actions
using ActionCallback = std::function<void(const Action &)>;

// Forward declaration of ActionHandler
class ActionHandler;

// Helper class to manage automatic registration and unregistration of action
// callbacks
template <typename T> class ActionRegister {
public:
  ActionRegister() = default;
  ~ActionRegister();

  inline auto register_action(const std::string &action_type,
                              void (T::*callback)(const Action &)) -> void;

  inline auto register_action(const std::string &action_type,
                              ActionCallback callback) -> void;

private:
  friend class ActionHandler;
  ActionRegister(ActionHandler &handler, T *instance)
      : handler_(handler), instance_(instance) {}

  ActionHandler &handler_;
  T *instance_;
  std::vector<ActionCallback> registered_callbacks_;
  std::vector<ActionCallback> registered_function_callbacks_;
};

class ActionHandler {
public:
  struct FilteredActionCallback {
    std::string action_type;
    ActionCallback callback;
  };

  explicit ActionHandler(EventHandler &event_handler)
      : event_handler_(event_handler) {
    event_handler_.register_callback(
        [](const Event &event) { return true; }, // Pass all events to process
        [this](const Event &event) { process_event(event); });
  }

  ~ActionHandler() = default;

  template <typename T> inline auto make_registry(T *ptr) -> ActionRegister<T> {
    return ActionRegister<T>(*this, ptr);
  }

  void register_action(const std::string &action_type, ActionFilter filter) {
    action_filters_[action_type] = std::move(filter);
  }

  void register_action_callback(const std::string &action_type,
                                ActionCallback callback) {
    action_callbacks_.emplace_back(
        FilteredActionCallback{action_type, std::move(callback)});
  }

  void unregister_action_callback(ActionCallback &callback) {
    action_callbacks_.erase(
        std::remove_if(action_callbacks_.begin(), action_callbacks_.end(),
                       [&callback](FilteredActionCallback &fac) {
                         return fac.callback.target<void(const Action &)>() ==
                                callback.target<void(const Action &)>();
                       }),
        action_callbacks_.end());
  }

  void process_event(const Event &event) {
    for (const auto &[action_type, filter] : action_filters_) {
      Action action{action_type, event.timestamp};
      if (filter(event, action)) {
        for (const auto &[registered_action_type, callback] :
             action_callbacks_) {
          if (action_type == registered_action_type) {
            callback(action);
          }
        }
      }
    }
  }

  static bool is_just_pressed(const Event &event, const Action &action) {
    return event.type == EventType::Pressed;
  }

  static bool is_just_released(const Event &event, const Action &action) {
    return event.type == EventType::Released;
  }

private:
  EventHandler &event_handler_; // Reference to the EventHandler
  std::unordered_map<std::string, ActionFilter>
      action_filters_; // Map of action types to filters
  std::vector<FilteredActionCallback>
      action_callbacks_; // List of registered action callbacks
};

template <typename T> ActionRegister<T>::~ActionRegister() {
  for (auto &cb : registered_callbacks_) {
    handler_.unregister_action_callback(cb);
  }

  for (auto &cb : registered_function_callbacks_) {
    handler_.unregister_action_callback(cb);
  }
}

template <typename T>
inline auto ActionRegister<T>::register_action(
    const std::string &action_type,
    void (T::*callback)(const Action &)) -> void {
  auto bound_callback = [this, callback](const Action &action) {
    (instance_->*callback)(action);
  };
  handler_.register_action_callback(action_type, bound_callback);
  registered_callbacks_.push_back(bound_callback);
}

template <typename T>
inline auto
ActionRegister<T>::register_action(const std::string &action_type,
                                   ActionCallback callback) -> void {
  handler_.register_action_callback(action_type, callback);
  registered_function_callbacks_.push_back(callback);
}

} // namespace meshi
