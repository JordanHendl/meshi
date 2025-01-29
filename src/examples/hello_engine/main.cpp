#include <iostream>
#include <meshi/bits/camera.hpp>
#include <meshi/bits/denizen.hpp>
#include <meshi/meshi.hpp>
class MyObject : public meshi::Denizen {
public:
  MyObject()
      : m_event(std::make_shared<meshi::ActionRegister<MyObject>>(
            meshi::engine()->action().make_registry(this))) {
    std::cout << "Making object!" << std::endl;

    // Subobjects make up things. Nice for grouping objects together.
    // Note: Transforms do not propagate for subobjects.
    m_cube = add_subobject<meshi::CubeMeshComponent>();
    m_camera = add_subobject<meshi::CameraComponent>();

    this->m_root_component = m_camera;
    m_cube->attach_to(m_camera);
    auto d = glm::mat4();
    m_event->register_action("Move Forward", &MyObject::move_camera_forward);
    m_event->register_action("Move Left", &MyObject::move_camera_left);
    m_event->register_action("Move Right", &MyObject::move_camera_right);
    m_event->register_action("Move Back", &MyObject::move_camera_back);

    this->activate();
  }

  auto update(float dt) -> void override {
    meshi::Denizen::update(dt);
    // Note: Denizens need to apply their own movement in update().
  }

  auto move_camera_forward(const meshi::Action &action) -> void {
    std::cout << "MOVE FORWARD" << action.type << std::endl;
    //    this->append_velocity();
  }

  auto move_camera_left(const meshi::Action &action) -> void {
    std::cout << "MOVE LEFT" << action.type << std::endl;
    //    this->append_velocity();
  }

  auto move_camera_right(const meshi::Action &action) -> void {
    std::cout << "MOVE RIGHT" << action.type << std::endl;
    //    this->append_velocity();
  }

  auto move_camera_back(const meshi::Action &action) -> void {
    std::cout << "MOVE BACK" << action.type << std::endl;
    //    this->append_velocity();
  }

private:
  std::shared_ptr<meshi::ActionRegister<MyObject>> m_event;
  meshi::CubeMeshComponent *m_cube = nullptr;
  meshi::CameraComponent *m_camera = nullptr;
};

////////////////////////////////////////////////////////////

class Application {
public:
  Application() : m_event(meshi::engine()->event().make_registry(this)) {
    // Register quit event
    m_event.register_event(
        [](auto &event) { return event.type == meshi::EventType::Quit; },
        [this](auto &event) {
          std::cout << "QUITTING" << std::endl;
          m_running = false;
        });

    // Move
    meshi::engine()->action().register_action(
        "Move Forward", [](const meshi::Event &event, meshi::Action &action) {
          if (meshi::ActionHandler::is_just_pressed(event, action)) {
            if (event.source == meshi::EventSource::Key &&
                event.payload.press.key == meshi::KeyCode::W) {
              action.type = "movement";
              return true;
            }
          }
          return false;
        });

    meshi::engine()->action().register_action(
        "Move Left", [](const meshi::Event &event, meshi::Action &action) {
          if (meshi::ActionHandler::is_just_pressed(event, action)) {
            if (event.source == meshi::EventSource::Key &&
                event.payload.press.key == meshi::KeyCode::A) {
              action.type = "movement";
              return true;
            }
          }
          return false;
        });

    meshi::engine()->action().register_action(
        "Move Right", [](const meshi::Event &event, meshi::Action &action) {
          if (meshi::ActionHandler::is_just_pressed(event, action)) {
            if (event.source == meshi::EventSource::Key &&
                event.payload.press.key == meshi::KeyCode::D) {
              action.type = "movement";
              return true;
            }
          }
          return false;
        });

    meshi::engine()->action().register_action(
        "Move Back", [](const meshi::Event &event, meshi::Action &action) {
          if (meshi::ActionHandler::is_just_pressed(event, action)) {
            if (event.source == meshi::EventSource::Key &&
                event.payload.press.key == meshi::KeyCode::S) {
              action.type = "movement";
              return true;
            }
          }
          return false;
        });

    meshi::engine()->world().spawn_object<MyObject>();
  }

  auto run() -> void {
    while (m_running) {
      meshi::engine()->update();
    }
  }

private:
  bool m_running = true;
  meshi::EventRegister<Application> m_event;
};

auto main() -> int {
  meshi::initialize_meshi_engine(meshi::EngineInfo{
      .application_name = "Hello Engine!",
      .application_root = ".",
  });

  auto app = Application();
  app.run();

  return 0;
}
