#include <pcg_random.hpp>
#include <random>

namespace Aux {

  pcg64 setup_random_number_generator();

  /// \brief provides a normal distribution sample on invocation.
  ///
  /// CAREFUL: This class is probably not thread safe, because pcg64 is not.
  /// At the time of writing this we create a random number generator per
  /// object, so this should be fine.
  class Normaldistribution final {
  public:
    Normaldistribution() = default;

    double operator()(double mean, double standard_deviation);
    double operator()(double standard_deviation);

  private:
    pcg64 generator{setup_random_number_generator()};
    std::normal_distribution<> dist;
  };
} // namespace Aux
