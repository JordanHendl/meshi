#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <meshi/bits/base.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
namespace meshi {
class Denizen : public Actor {
public:
  Denizen() = default;
  virtual ~Denizen() = default;

  // Override the update function to handle movement
  auto apply_movement(float dt) -> void {
    // Apply velocity dampening
    m_velocity *= glm::pow(1.0f - m_dampening_factor, dt);

    // Update position using velocity
    m_transform = glm::translate(m_transform, m_velocity * dt);

    // Check collisions and resolve them
    resolve_collisions();
  }

  // Set velocity
  inline auto set_velocity(const glm::vec3 &velocity) -> void {
    m_velocity = velocity;
  }
  
  inline auto append_velocity(const glm::vec3& velocity) -> void {
    m_velocity += velocity;
  }

  // Get velocity
  inline auto velocity() -> const glm::vec3 & { return m_velocity; }

  // Set dampening factor
  inline auto set_dampening_factor(float dampening) -> void {
    m_dampening_factor = dampening;
  }

  // Get dampening factor
  inline auto dampening_factor() -> float { return m_dampening_factor; }

  // Add a collider (e.g., a bounding box)
  inline auto add_collider(const glm::vec3 &min, const glm::vec3 &max) -> void {
    m_colliders.push_back({min, max});
  }

  // Remove all colliders
  inline auto clear_colliders() -> void { m_colliders.clear(); }

  // Check if colliding with another Denizen
  inline auto is_colliding_with(const Denizen &other) -> bool {
    for (const auto &collider : m_colliders) {
      for (const auto &other_collider : other.m_colliders) {
        if (check_collision(collider, other_collider)) {
          return true;
        }
      }
    }
    return false;
  }

private:
  struct Collider {
    glm::vec3 min;
    glm::vec3 max;
  };

  glm::vec3 m_velocity = {0.0f, 0.0f, 0.0f};
  float m_dampening_factor = 0.1f; // Default dampening factor
  std::vector<Collider> m_colliders;

  // Resolve collisions (simplified example)
  auto resolve_collisions() -> void {
    // For simplicity, this example assumes collisions are resolved by stopping
    // the Denizen.
    for (const auto &collider : m_colliders) {
      for (const auto &other : m_potential_collisions) {
        for (const auto &other_collider : other->m_colliders) {
          if (check_collision(collider, other_collider)) {
            m_velocity = glm::vec3(0.0f); // Stop movement
            return;
          }
        }
      }
    }
  }

  // Basic AABB collision check
  static auto check_collision(const Collider &a, const Collider &b) -> bool {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
  }

  // List of other Denizens to check collisions with
  std::vector<Denizen *> m_potential_collisions;

public:
  // Add a potential collision target
  inline auto add_potential_collision(Denizen *other) -> void {
    if (std::find(m_potential_collisions.begin(), m_potential_collisions.end(),
                  other) == m_potential_collisions.end()) {
      m_potential_collisions.push_back(other);
    }
  }

  // Remove a potential collision target
  inline auto remove_potential_collision(Denizen *other) -> void {
    m_potential_collisions.erase(std::remove(m_potential_collisions.begin(),
                                             m_potential_collisions.end(),
                                             other),
                                 m_potential_collisions.end());
  }
};
} // namespace meshi
