#pragma once
#include <cassert>
#include <iostream>
#include <variant>
namespace meshi {
// A result wrapper class.
template <typename T, typename E> class Result {
public:
  Result() { _data = E(); }
  template <typename Other>
  Result(const Result<Other, E> &other) : _data(other.err()) {}
  Result(T &&val) : _data(std::move(val)) {}
  Result(E err) : _data(err) {}
  template <typename Other>
  inline auto operator=(const Result<Other, E> &other) -> Result<T, E> & {
    _data = other.err();
    return *this;
  }

  inline auto is_err() const -> bool { return _data.index() == 1; }
  inline auto is_ok() const -> bool { return !is_err(); }
  inline auto unwrap() -> T && {
    if (!is_ok())
      std::cout << err().to_string() << std::endl;

    assert(is_ok());
    return std::move(std::get<0>(_data));
  }

  inline auto err() const -> E {
    assert(is_err());
    return std::move(std::get<1>(_data));
  }

private:
  std::variant<T, E> _data;
};

template <typename T, typename E>
inline auto make_result(T &&type) -> Result<T, E> {
  return Result<T, E>(std::move(type));
}

template <typename E> inline auto make_error(E err) -> Result<int, E> {
  return Result<int, E>(err);
}

#define MESHI_CHECK_ERROR(result_expr)                                         \
  ({                                                                           \
    auto _meshi_result = (result_expr);                                        \
    if (_meshi_result.is_err())                                                \
      return meshi::make_error(_meshi_result.err());                           \
    std::move(_meshi_result.unwrap());                                         \
  })

} // namespace meshi
