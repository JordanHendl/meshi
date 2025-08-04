#pragma once
#include "meshi/backend.hpp"
#include <algorithm>
#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <vector>
namespace meshi {
using vec2 = glm::vec2;

enum class EventType {
  Unknown = 0,
  Quit = 1,
  Pressed = 2,
  Released = 3,
  Joystick = 4,
  Motion2D = 5,
};

enum class EventSource {
  Unknown = 0,
  Key = 1,
  Mouse = 2,
  MouseButton = 3,
  Gamepad = 4,
};

enum class KeyCode {
  // Alphanumeric keys
  A = 0,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  J,
  K,
  L,
  M,
  N,
  O,
  P,
  Q,
  R,
  S,
  T,
  U,
  V,
  W,
  X,
  Y,
  Z,

  // Number keys (top row)
  Digit0,
  Digit1,
  Digit2,
  Digit3,
  Digit4,
  Digit5,
  Digit6,
  Digit7,
  Digit8,
  Digit9,

  // Function keys
  F1,
  F2,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  F10,
  F11,
  F12,

  // Modifier keys
  Shift,
  Control,
  Alt,
  Meta, // Windows key or Command key (Mac)

  // Navigation keys
  ArrowUp,
  ArrowDown,
  ArrowLeft,
  ArrowRight,

  // Special keys
  Escape,
  Enter,
  Space,
  Tab,
  Backspace,
  Delete,
  Insert,
  Home,
  End,
  PageUp,
  PageDown,

  // Punctuation and symbols
  Minus,        // -
  Equals,       // =
  LeftBracket,  // [
  RightBracket, // ]
  Backslash,    // \
  Semicolon,    // ;
  Apostrophe,   // '
  Comma,        // ,
  Period,       // .
  Slash,        // /
  GraveAccent,  // `

  // Numpad keys
  Numpad0,
  Numpad1,
  Numpad2,
  Numpad3,
  Numpad4,
  Numpad5,
  Numpad6,
  Numpad7,
  Numpad8,
  Numpad9,
  NumpadAdd,      // +
  NumpadSubtract, // -
  NumpadMultiply, // *
  NumpadDivide,   // /
  NumpadDecimal,  // .
  NumpadEnter,

  // Lock keys
  CapsLock,
  NumLock,
  ScrollLock,

  // Miscellaneous keys
  PrintScreen,
  Pause,
  Menu,

  // Undefined or custom keys
  Undefined
};
enum class MouseButton {
  Left,
  Right,
};

struct MouseButtonPayload {
  MouseButton button;
  glm::vec2 position;
};

struct PressPayload {
  KeyCode key;
  EventType previous;
};

struct Motion2DPayload {
  vec2 motion;
};
union Payload {
  PressPayload press;
  Motion2DPayload motion2d;
  MouseButtonPayload mouse_button;
};

struct Event {
  EventType type;
  EventSource source;
  Payload payload;
  uint32_t timestamp;
};

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
    engine->register_event_callback(
        this,
        [](MeshiEvent *ev, void *user_data) {
          Event e{};
          e.type = static_cast<EventType>(ev->event_type);
          e.source = static_cast<EventSource>(ev->source);
          e.timestamp = ev->timestamp;
          switch (e.type) {
          case EventType::Pressed:
          case EventType::Released:
            e.payload.press.key = static_cast<KeyCode>(ev->payload.press.key);
            e.payload.press.previous =
                static_cast<EventType>(ev->payload.press.previous);
            break;
          case EventType::Motion2D:
            e.payload.motion2d.motion =
                {ev->payload.motion2d.motion.x, ev->payload.motion2d.motion.y};
            break;
          default:
            break;
          }
          if (e.source == EventSource::MouseButton) {
            e.payload.mouse_button.button =
                static_cast<MouseButton>(ev->payload.mouse_button.button);
            e.payload.mouse_button.position =
                {ev->payload.mouse_button.pos.x, ev->payload.mouse_button.pos.y};
          }
          static_cast<EventHandler *>(user_data)->process_event(e);
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

    if (event.source == EventSource::MouseButton) {
      MouseButton button = event.payload.mouse_button.button;
      if (event.type == EventType::Pressed) {
        pressed_buttons_[button] = true;
      } else if (event.type == EventType::Released) {
        pressed_buttons_[button] = false;
      }
    } else if (event.source == EventSource::Key) {
      KeyCode key = event.payload.press.key;
      if (event.type == EventType::Pressed) {
        pressed_keys_[key] = true;
      } else if (event.type == EventType::Released) {
        pressed_keys_[key] = false;
      }
    }
  }

  bool is_pressed(MouseButton button) const {
    auto it = pressed_buttons_.find(button);
    return it != pressed_buttons_.end() && it->second;
  }

  bool is_released(MouseButton button) const {
    auto it = pressed_buttons_.find(button);
    return it != pressed_buttons_.end() && !it->second;
  }

  bool is_pressed(KeyCode key) const {
    auto it = pressed_keys_.find(key);
    return it != pressed_keys_.end() && it->second;
  }

  bool is_released(KeyCode key) const {
    auto it = pressed_keys_.find(key);
    return it != pressed_keys_.end() && !it->second;
  }

private:
  EngineBackend *engine_; // Pointer to the Meshi engine backend
  std::unordered_map<MouseButton, bool> pressed_buttons_;
  std::unordered_map<KeyCode, bool> pressed_keys_;
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
