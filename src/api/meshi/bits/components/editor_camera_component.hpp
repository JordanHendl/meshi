#pragma once
#include "meshi/bits/action.hpp"
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/ext/matrix_clip_space.hpp"
#include <meshi/bits/components/camera_component.hpp>
#include <meshi/engine.hpp>
namespace meshi {
class EditorCameraComponent;
class EditorCameraComponent : public CameraComponent {
public:
  static constexpr auto MOVEMENT_SPEED = 60.0;
  static constexpr auto ROTATION_SPEED = 250.0;
  EditorCameraComponent(float movement_speed = MOVEMENT_SPEED,
                        float rotation_speed = ROTATION_SPEED)
      : CameraComponent() {
    m_event = std::make_shared<meshi::ActionRegister<EditorCameraComponent>>(
        meshi::engine()->action().make_registry(this));

    m_movement_speed = movement_speed;
    m_rotation_speed = rotation_speed;
    // Register Actions to enable reacting to input.
    engine()->action().register_action(
        "Editor-Camera-Mouse",
        [](const meshi::Event &event, meshi::Action &action) {
          if (event.source == meshi::EventSource::Mouse &&
              event.type == meshi::EventType::Motion2D) {
            action.type = "movement";
            return true;
          }
          return false;
        });

    m_event->register_action("Editor-Camera-Mouse",
                             &EditorCameraComponent::handle_mouse_motion);
  }

  inline auto handle_mouse_motion(const meshi::Action &event) -> void {
    if (m_pressed) {
      const auto rotation_speed =
          m_rotation_speed * meshi::engine()->delta_time();
      auto offsets = event.event.payload.motion2d.motion; // vec2 mouse offsets

      // Extract yaw (horizontal) and pitch (vertical) rotation from mouse
      // movement
      float yaw = offsets.x * rotation_speed;
      float pitch = offsets.y * rotation_speed;

      // Get the current camera transformation matrix
      auto transform = world_transform();

      // Rotate around the Y-axis for yaw (horizontal movement)
      transform = glm::rotate(transform, glm::radians(yaw),
                              glm::vec3(0.0f, 1.0f, 0.0f));

      // Rotate around the X-axis for pitch (vertical movement)
      transform = glm::rotate(transform, glm::radians(pitch),
                              glm::vec3(1.0f, 0.0f, 0.0f));

      set_transform(transform);
    }
  }

  virtual ~EditorCameraComponent(){};

  virtual auto update(float dt) -> void override {
    CameraComponent::update(dt);
    if (engine()->event().is_pressed(MouseButton::Right)) {
      m_pressed = true;
      engine()->backend().graphics().capture_mouse(true);
    } else {
      engine()->backend().graphics().capture_mouse(false);
      m_pressed = false;
    }

    auto &e = engine()->event();

    if (e.is_pressed(KeyCode::W)) {
      move_camera_forward();
    }
    if (e.is_pressed(KeyCode::S)) {
      move_camera_back();
    }

    if (e.is_pressed(KeyCode::A)) {
      move_camera_left();
    }
    if (e.is_pressed(KeyCode::D)) {
      move_camera_right();
    }
  }

  auto move_camera_forward() -> void {
    auto translation = this->front() * glm::vec3(m_movement_speed *
                                                 meshi::engine()->delta_time());

    auto transform = glm::translate(world_transform(), translation);
    set_transform(transform);
  }

  auto move_camera_left() -> void {
    auto translation =
        -this->right() *
        glm::vec3(m_movement_speed * meshi::engine()->delta_time());
    auto transform = glm::translate(world_transform(), translation);
    set_transform(transform);
  }

  auto move_camera_right() -> void {
    auto translation = this->right() * glm::vec3(m_movement_speed *
                                                 meshi::engine()->delta_time());
    auto transform = glm::translate(world_transform(), translation);
    set_transform(transform);
  }

  auto move_camera_back() -> void {
    auto translation =
        -this->front() *
        glm::vec3(m_movement_speed * meshi::engine()->delta_time());
    auto transform = glm::translate(world_transform(), translation);
    set_transform(transform);
  }

private:
  float m_movement_speed;
  float m_rotation_speed;
  std::shared_ptr<meshi::ActionRegister<EditorCameraComponent>> m_event;
  bool m_pressed;
};
} // namespace meshi
