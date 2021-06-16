#include <liberrors/errors.hxx>
#define BOOST_STACKTRACE_USE_BACKTRACE
#include <boost/stacktrace.hpp>
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

            auto style = fg(fmt::color::crimson) | fmt::emphasis::bold;
            auto assert_fmt
                = fmt::format(style, FMT_STRING(" {} : {}\n"), id_error, msg);
            auto loc_info
                = fmt::format(FMT_COMPILE("@ in file:{} at line {}:{}\n"),
                              loc.file_name(), loc.line(), loc.column());
            auto func_proto = fmt::format(FMT_COMPILE("in function {}\n"),
                                          loc.function_name());
            auto bt_style = fmt::emphasis::bold;
            auto backtrace = fmt::format(
                bt_style, FMT_STRING("with backtrace:\n{}\n"),
                boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
            return fmt::format(FMT_COMPILE("{} {} {} {}"), assert_fmt,
                               loc_info, func_proto, backtrace);
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
        std::abort();
      }
  }

  recoverable_err::recoverable_err(std::string_view msg,
                                   std::source_location loc) noexcept
      : message{ format_exception(msg, loc) }
  {
  }

  auto
  recoverable_err::what() const noexcept -> const char *
  {
    return message.c_str();
  }
} // namespace errors
