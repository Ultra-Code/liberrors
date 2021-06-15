#pragma once
#include <source_location>
#include <string_view>
#include <stdexcept>

#include <liberrors/export.hxx>

namespace errors
{

  [[noreturn]] auto
  assert_that(bool condition, std::string_view assert_msg,
              std::source_location loc
              = std::source_location::current()) noexcept -> void;

  auto
  trace_on_abort() -> void;

  class recoverable_err : std::exception
  {
    std::string message;

  public:
    explicit recoverable_err(std::string_view msg,
                           std::source_location loc
                           = std::source_location::current()) noexcept;
    [[nodiscard]] auto
    what() const noexcept -> const char * override;
  };
} // namespace errors
