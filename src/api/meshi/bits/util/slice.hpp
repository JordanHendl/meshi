#pragma once
#include <array>
#include <cassert>
#include <vector>
namespace meshi {

// Span-like class.
// See https://en.cppreference.com/w/cpp/container/span
template <typename T> class Slice {
public:
  Slice() = default;
  ~Slice() = default;
  Slice(T *ptr, std::size_t sz) : m_data(ptr), _sz(sz) {}
  template <std::size_t SZ>
  Slice(std::array<T, SZ> &arr) : m_data(arr.data()), _sz(arr.size()) {}
  Slice(std::vector<T> &vec) : m_data(vec.data()), _sz(vec.size()) {}

  inline auto begin() -> T* { return m_data; }
  inline auto end() -> T * { return begin() + size(); }
  inline auto cbegin() -> const T* { return m_data; }
  inline auto cend() -> const T* { return begin() + size(); }

  [[nodiscard]] inline auto size() const -> std::size_t { return _sz; }
  inline auto data() -> T * { return m_data; }
  inline auto empty() const -> bool { return _sz == 0; }

  inline auto operator[](std::size_t idx) -> T & {
    assert(idx < _sz);
    return m_data[idx];
  }

  inline auto operator=(std::vector<T> &vec) -> Slice & {
    m_data = vec.data();
    _sz = vec.size();
    return *this;
  }

  template <std::size_t SZ>
  static inline auto from_raw(T (&a)[SZ]) -> Slice<T> {
    return Slice<T>(&a[0], SZ);
  }

  template <typename OtherType> inline auto reinterpret() -> Slice<OtherType> {
    const auto new_ptr = reinterpret_cast<OtherType *>(m_data);
    const auto new_sz = sizeof(OtherType) < sizeof(T) ? _sz * sizeof(OtherType)
                                                      : _sz / sizeof(OtherType);

    return Slice<OtherType>(new_ptr, new_sz);
  }

private:
  T *m_data = nullptr;
  std::size_t _sz = 0;
};

template <template <typename> class C, typename T>
static inline auto slice_from_container(C<T> &c) -> Slice<T> {
  return Slice<T>(c.data(), c.size());
}
} // namespace meshi
