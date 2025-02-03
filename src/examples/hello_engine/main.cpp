#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <meshi/bits/components/camera_component.hpp>
#include <meshi/bits/components/cube_mesh_component.hpp>
#include <meshi/bits/objects/denizen.hpp>
#include <meshi/meshi.hpp>

#include "example_helper.hpp"
#include "meshi/bits/components/editor_camera_component.hpp"

class Cube : public meshi::Actor {
public:
  Cube() {
    add_subobject<meshi::MeshComponent>(meshi::MeshComponent::CreateInfo{
                                            .render_info =
                                                {
                                                    .mesh = "witch.body",
                                                },
                                            .rigid_body_info = {},
                                        })
        ->attach_to(root_component());

    add_subobject<meshi::MeshComponent>(meshi::MeshComponent::CreateInfo{
                                            .render_info =
                                                {
                                                    .mesh = "witch.hair",
                                                },
                                            .rigid_body_info = {},
                                        })
        ->attach_to(root_component());

    add_subobject<meshi::MeshComponent>(meshi::MeshComponent::CreateInfo{
                                            .render_info =
                                                {
                                                    .mesh = "witch.clothes",
                                                },
                                            .rigid_body_info = {},
                                        })
        ->attach_to(root_component());

    m_cube =
        add_subobject<meshi::MeshComponent>(meshi::MeshComponent::CreateInfo{
            .render_info =
                {
                    .mesh = "witch.hat",
                },
            .rigid_body_info = {},
        });

    m_cube->attach_to(root_component());
  }

private:
  meshi::MeshComponent *m_cube = nullptr;
};

class MyObject : public meshi::Denizen {
public:
  MyObject()
      : m_event(std::make_shared<meshi::ActionRegister<MyObject>>(
            meshi::engine()->action().make_registry(this))) {
    // Subobjects make up things. Nice for grouping objects together.
    // Note: Transforms do not propagate for subobjects.
    m_camera = add_subobject<meshi::EditorCameraComponent>();
    // Attach components to our root.
    m_camera->attach_to(root_component());
    m_camera->apply_to_world();

    auto initial_transform = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 5.0, 30.0));
    initial_transform = glm::rotate(initial_transform, (float)glm::radians(0.0), this->up());
    m_camera->set_transform(initial_transform);
  }

  auto update(float dt) -> void override { meshi::Denizen::update(dt); }
private:
  std::shared_ptr<meshi::ActionRegister<MyObject>> m_event;
  meshi::EditorCameraComponent *m_camera = nullptr;
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
    meshi::engine()->action().register_action(
        "Rotate Up", [](const meshi::Event &event, meshi::Action &action) {
          if (meshi::ActionHandler::is_just_pressed(event, action)) {
            if (event.source == meshi::EventSource::Key &&
                event.payload.press.key == meshi::KeyCode::ArrowUp) {
              action.type = "movement";
              return true;
            }
          }
          return false;
        });

    meshi::engine()->action().register_action(
        "Rotate Down", [](const meshi::Event &event, meshi::Action &action) {
          if (meshi::ActionHandler::is_just_pressed(event, action)) {
            if (event.source == meshi::EventSource::Key &&
                event.payload.press.key == meshi::KeyCode::ArrowDown) {
              action.type = "movement";
              return true;
            }
          }
          return false;
        });

    meshi::engine()->action().register_action(
        "Rotate Left", [](const meshi::Event &event, meshi::Action &action) {
          if (meshi::ActionHandler::is_just_pressed(event, action)) {
            if (event.source == meshi::EventSource::Key &&
                event.payload.press.key == meshi::KeyCode::ArrowLeft) {
              action.type = "movement";
              return true;
            }
          }
          return false;
        });

    meshi::engine()->action().register_action(
        "Rotate Right", [](const meshi::Event &event, meshi::Action &action) {
          if (meshi::ActionHandler::is_just_pressed(event, action)) {
            if (event.source == meshi::EventSource::Key &&
                event.payload.press.key == meshi::KeyCode::ArrowRight) {
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
      .application_root = EXAMPLE_APP_DIR,
  });

  auto app = Application();
  app.run();

  return 0;
}
