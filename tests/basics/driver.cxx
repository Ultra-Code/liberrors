#include <liberrors/version.hxx>
#include <liberrors/errors.hxx>
#include <ut.hpp>

using namespace boost::ut;

auto
asserting() -> void
{
  errors::assert_that(1 == 2, "1 is not equal to 2");
}
auto
main() -> int
{
  try
    {
      throw errors::recoverable_err("Recover from nasty error");
    }
  catch (errors::recoverable_err const &ex)
    {
      std::cerr << ex.what();
      ex.print_stacktrace(std::cerr);
    }
  catch (std::exception const &ex)
    {
      std::cerr << "Using default \n";
      std::cerr << ex.what();
    }
  catch (...)
    {
      std::cerr << "Fuck this shit\n";
    }
}
