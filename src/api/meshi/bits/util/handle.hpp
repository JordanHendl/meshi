#pragma once
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <limits>
#include <memory>
#include <optional>
#include <vector>
namespace meshi {
template <typename T> struct Handle {
  uint16_t slot = std::numeric_limits<std::uint16_t>::max();
  uint16_t generation = std::numeric_limits<std::uint16_t>::max();

  bool valid() const { return slot != UINT16_MAX && generation != UINT16_MAX; }

  bool operator==(const Handle &other) const {
    return slot == other.slot && generation == other.generation;
  }
};

template <typename T> class ItemList {
private:
  std::unique_ptr<T[]> items;
  size_t capacity;
  bool imported;

public:
  explicit ItemList(size_t len)
      : items(std::make_unique<T[]>(len)), capacity(len), imported(false) {}

  ItemList(T *ptr, size_t len) : items(ptr), capacity(len), imported(true) {}

  size_t size() const { return capacity; }

  T *data() { return items.get(); }
  const T *data() const { return items.get(); }

  void expand(size_t amt) {
    if (!imported) {
      size_t new_capacity = capacity + amt;
      std::unique_ptr<T[]> new_items = std::make_unique<T[]>(new_capacity);
      std::memcpy(new_items.get(), items.get(), capacity * sizeof(T));
      items = std::move(new_items);
      capacity = new_capacity;
    }
  }

  T &operator[](size_t index) { return items[index]; }
  const T &operator[](size_t index) const { return items[index]; }
};

template <typename T> class Pool {
private:
  ItemList<T> items;
  std::vector<uint32_t> empty;
  std::vector<uint16_t> generation;

public:
  explicit Pool(size_t initial_size = 1024)
      : items(initial_size), generation(initial_size, 0) {
    for (uint32_t i = 0; i < initial_size; ++i) {
      empty.push_back(i);
    }
  }

  Handle<T> insert(const T &item) {
    if (empty.empty()) {
      expand(1024);
    }

    uint32_t empty_slot = empty.back();
    empty.pop_back();

    items[empty_slot] = item;

    return Handle<T>{static_cast<uint16_t>(empty_slot), generation[empty_slot]};
  }

  void expand(size_t amount) {
    size_t old_size = items.size();
    items.expand(amount);
    generation.resize(items.size(), 0);

    for (size_t i = old_size; i < items.size(); ++i) {
      empty.push_back(static_cast<uint32_t>(i));
    }
  }

  std::optional<T *> get_ref(const Handle<T> &handle) {
    if (handle.valid() && generation[handle.slot] == handle.generation) {
      return &items[handle.slot];
    }
    return std::nullopt;
  }

  void release(const Handle<T> &handle) { empty.push_back(handle.slot); }

  void clear() {
    empty.clear();
    for (size_t i = 0; i < items.size(); ++i) {
      empty.push_back(static_cast<uint32_t>(i));
    }
    std::fill(generation.begin(), generation.end(), 0);
  }
};
} // namespace meshi
