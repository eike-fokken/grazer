#pragma once
namespace Aux {
  class Normaldistribution;

  double euler_maruyama_step(
      double last_value, double theta, double mu, double delta_t, double sigma,
      Normaldistribution &distribution);

} // namespace Aux
