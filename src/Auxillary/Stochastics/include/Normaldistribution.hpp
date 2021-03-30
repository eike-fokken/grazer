#include <pcg_random.hpp>
#include <random>

namespace Aux {

  pcg64 setup_random_number_generator();

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
