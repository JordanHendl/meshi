#pragma once
#include "meshi/meshi_c_api.h"
#include <algorithm>
#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <vector>
namespace meshi {

// Define a filter function type for events
using EventFilter = std::function<bool(const Event &)>;

// Define a callback function type for events
using EventCallback = std::function<void(const Event &)>;

class EventHandler;

// Helper class to manage automatic registration and unregistration of callbacks
template <typename T> class EventRegister {
public:
  EventRegister() = default;
  ~EventRegister();
  inline auto register_event(EventFilter filter,
                             void (T::*callback)(const Event &)) -> void;

  inline auto register_event(EventFilter filter,
                             EventCallback callback) -> void;

private:
  friend class EventHandler;
  EventRegister(EventHandler &handler, T *instance)
      : handler_(handler), instance_(instance) {}
  EventHandler &handler_;
  T *instance_;
  std::vector<EventCallback> registered_callbacks_;
  std::vector<EventCallback> registered_function_callbacks_;
};

class EventHandler {
public:
  // Structure to hold a filter and its corresponding callback
  struct FilteredCallback {
    EventFilter filter;
    EventCallback callback;
  };

  // Constructor to initialize the event handler with the Meshi engine
  explicit EventHandler(EngineBackend *engine) : engine_(engine) {
    // Register the global callback function to the engine
    meshi_register_event_callback(
        engine_,
        this, // Pass the instance as user_data
        [](Event &event, void *user_data) {
          static_cast<EventHandler *>(user_data)->process_event(event);
        });
  }

  // Destructor
  ~EventHandler() = default;

  template <typename T> inline auto make_registry(T *ptr) -> EventRegister<T> {
    return EventRegister<T>(*this, ptr);
  }

  // Register a new callback with a filter
  void register_callback(EventFilter filter, EventCallback callback) {
    callbacks_.emplace_back(
        FilteredCallback{std::move(filter), std::move(callback)});
  }

  // Unregister a callback by providing its pointer
  void unregister_callback(EventCallback &callback) {
    callbacks_.erase(
        std::remove_if(callbacks_.begin(), callbacks_.end(),
                       [&callback](FilteredCallback &fc) {
                         return fc.callback.target<void(const Event &)>() ==
                                callback.target<void(const Event &)>();
                       }),
        callbacks_.end());
  }

  // Process an event (called by the global callback)
  void process_event(const Event &event) {
    for (const auto &[filter, callback] : callbacks_) {
      if (filter(event)) {
        callback(event);
      }
    }
  }

private:
  EngineBackend *engine_; // Pointer to the Meshi engine backend
  std::vector<FilteredCallback>
      callbacks_; // List of registered callbacks with filters
};

template <typename T> EventRegister<T>::~EventRegister() {
  for (auto &cb : registered_callbacks_) {
    handler_.unregister_callback(cb);
  }

  for (auto &cb : registered_function_callbacks_) {
    handler_.unregister_callback(cb);
  }
}

template <typename T>
inline auto
EventRegister<T>::register_event(EventFilter filter,
                                 void (T::*callback)(const Event &)) -> void {
  auto bound_callback = [this, callback](const Event &event) {
    (instance_->*callback)(event);
  };
  handler_.register_callback(filter, bound_callback);
  registered_callbacks_.push_back(bound_callback);
}

template <typename T>
inline auto EventRegister<T>::register_event(EventFilter filter,
                                             EventCallback callback) -> void {
  handler_.register_callback(filter, callback);
  registered_function_callbacks_.push_back(callback);
}

} // namespace meshi
