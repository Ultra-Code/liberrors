#include <liberrors/errors.hxx>
#define BACKWARD_HAS_DW 1
#include <backward.hpp>
#include <fmt/compile.h>
#include <fmt/color.h>
#include <iostream>
#include <string_view>

namespace errors
{

  namespace
  {
    auto
    format_error(std::string_view id_error) noexcept
    {
      return [id_error](std::string_view msg,
                        std::source_location loc) -> std::string {
        try
          {
            auto assert_style = fg(fmt::color::crimson) | fmt::emphasis::bold;
            auto assert_fmt = fmt::format(
                assert_style, FMT_STRING(" {} : {}\n"), id_error, msg);
            auto loc_style = fmt::fg(fmt::color::forest_green);
            auto loc_info = fmt::format(loc_style, FMT_STRING("@ in file:{}"),
                                        loc.file_name());
            auto line_info = fmt::format(FMT_STRING(" at line {}:{}\n"),
                                         loc.line(), loc.column());
            auto func_proto_style = fmt::fg(fmt::color::yellow);
            auto func_proto
                = fmt::format(func_proto_style, FMT_STRING("in function {}\n"),
                              loc.function_name());
            return fmt::format(FMT_COMPILE("{} {} {} {}"), assert_fmt,
                               loc_info, line_info, func_proto);
          }
        catch (fmt::format_error const &fmt_err)
          {
            constexpr auto loc = std::source_location::current();
            std::cerr << "Format error in " << loc.file_name() << "\n"
                      << "@ line " << loc.line() << " : " << loc.column()
                      << " of " << loc.function_name() << '\n';
            std::cerr << "With message : " << fmt_err.what() << '\n';
            return "Error occurred when formating error string\n";
          }
      };
    }
    const auto format_assertion = format_error("Assertion failed");
    const auto format_exception
        = format_error("Exception thrown with message");

  } // namespace

  auto
  assert_that(bool condition, std::string_view assert_msg,
              std::source_location loc) noexcept -> void
  {
    if (not condition)
      {
        std::cerr << format_assertion(assert_msg, loc);
        using namespace backward;
        StackTrace st;
        st.load_here();
        Printer p;
        p.object = true;
        p.color_mode = ColorMode::always;
        p.address = true;
        p.print(st, stderr);
        std::abort();
      }
  }

  recoverable_err::recoverable_err(std::string_view msg,
                                   std::source_location loc) noexcept
      : std::runtime_error{ format_exception(msg, loc) }
      , stacktrace_{ backward::StackTrace() }
  {
    stacktrace_.load_here();
  }

  auto
  recoverable_err::print_stacktrace(std::ostream &stream)const  noexcept -> void
  {
    using namespace backward;
    Printer p;
    p.object = true;
    p.color_mode = ColorMode::always;
    p.address = true;
    p.print(stacktrace_, stream);
  }

  auto
  recoverable_err::what() const noexcept -> const char *
  {
    return std::runtime_error::what();
  }
} // namespace errors
