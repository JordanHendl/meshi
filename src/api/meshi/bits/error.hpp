#pragma once
#include <meshi/bits/util/result.hpp>
#include <string>
#include <string_view>
namespace meshi {
enum class ErrorCode {
  None,
};

struct Error {
  ErrorCode code = ErrorCode::None;
  inline auto to_string() -> std::string_view {
    switch (code) {
    case ErrorCode::None:
      return "None";
      break;
    }
  }
};
} // namespace meshi
