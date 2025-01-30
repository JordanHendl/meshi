#include <iostream>
#include <meshi/bits/components/camera_component.hpp>
#include <meshi/bits/components/cube_mesh_component.hpp>
#include <meshi/bits/objects/denizen.hpp>
#include <meshi/meshi.hpp>

class Cube : public meshi::Actor {
public:
  Cube() {
    add_subobject<meshi::MeshComponent>(meshi::MeshComponent::CreateInfo{
                                            .render_info =
                                                {
                                                    .mesh = "witch.Cesium_Milk_Truck",
                                                },
                                            .rigid_body_info = {},
                                        })
        ->attach_to(root_component());

    add_subobject<meshi::MeshComponent>(meshi::MeshComponent::CreateInfo{
                                            .render_info =
                                                {
                                                    .mesh = "witch.Wheels",
                                                },
                                            .rigid_body_info = {},
                                        })
        ->attach_to(root_component());

//    add_subobject<meshi::MeshComponent>(meshi::MeshComponent::CreateInfo{
//                                            .render_info =
//                                                {
//                                                    .mesh = "witch.Cube.003",
//                                                },
//                                            .rigid_body_info = {},
//                                        })
//        ->attach_to(root_component());
//
//    m_cube =
//        add_subobject<meshi::MeshComponent>(meshi::MeshComponent::CreateInfo{
//            .render_info =
//                {
//                    .mesh = "witch.Cube.014",
//                },
//            .rigid_body_info = {},
//        });
//
//    m_cube->attach_to(root_component());
  }

private:
  meshi::MeshComponent *m_cube = nullptr;
};

class MyObject : public meshi::Denizen {
public:
  MyObject()
      : m_event(std::make_shared<meshi::ActionRegister<MyObject>>(
            meshi::engine()->action().make_registry(this))) {
    std::cout << "Making object!" << std::endl;

    // Subobjects make up things. Nice for grouping objects together.
    // Note: Transforms do not propagate for subobjects.
    m_camera = add_subobject<meshi::CameraComponent>();
    // Attach components to our root.
    m_camera->attach_to(root_component());
    m_camera->apply_to_world();

    m_event->register_action("Move Forward", &MyObject::move_camera_forward);
    m_event->register_action("Move Left", &MyObject::move_camera_left);
    m_event->register_action("Move Right", &MyObject::move_camera_right);
    m_event->register_action("Move Back", &MyObject::move_camera_back);
  }

  auto update(float dt) -> void override { meshi::Denizen::update(dt); }

  static constexpr auto MOVEMENT_SPEED = 10.0;
  auto move_camera_forward(const meshi::Action &action) -> void {
    auto translation = this->front() * glm::vec3(MOVEMENT_SPEED);
    auto transform = glm::translate(m_camera->world_transform(), translation);
    m_camera->set_transform(transform);
  }

  auto move_camera_left(const meshi::Action &action) -> void {
    auto translation = -this->right() * glm::vec3(MOVEMENT_SPEED);
    auto transform = glm::translate(m_camera->world_transform(), translation);
    m_camera->set_transform(transform);
  }

  auto move_camera_right(const meshi::Action &action) -> void {
    auto translation = this->right() * glm::vec3(MOVEMENT_SPEED);
    auto transform = glm::translate(m_camera->world_transform(), translation);
    m_camera->set_transform(transform);
  }

  auto move_camera_back(const meshi::Action &action) -> void {
    auto translation = -this->front() * glm::vec3(MOVEMENT_SPEED);
    auto transform = glm::translate(m_camera->world_transform(), translation);
    m_camera->set_transform(transform);
  }

private:
  std::shared_ptr<meshi::ActionRegister<MyObject>> m_event;
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

    // Register Actions to enable reacting to input.
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

    // Spawn our object, and activate it.
    meshi::engine()->world().spawn_object<Cube>()->activate();
    meshi::engine()->world().spawn_object<MyObject>()->activate();
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
