#pragma once
#include <array>
#include <pcg_random.hpp>
#include <random>

namespace Aux {
  static unsigned long constexpr pcg_seed_count = 8;

  pcg64 setup_random_number_generator(std::array<uint32_t, pcg_seed_count> &);

  std::array<uint32_t, pcg_seed_count> make_random_seed();
  /** \brief provides a normal distribution sample on invocation.
   *
   * CAREFUL: This class is probably not thread safe, because pcg64 may not
   * be thread safe. Also randutils is DEFINITELY not thread safe! At the
   * time of writing this we create a random number generator per object, so
   * this should be fine.
   */
  class Normaldistribution final {
  public:
    Normaldistribution(
        std::array<uint32_t, pcg_seed_count> &used_seed,
        std::array<uint32_t, pcg_seed_count> seed = make_random_seed());

    /** @brief Obtain a sample of a normal distribution.
     *
     * @returns sample
     * @param mean Mean of the  normal distribution.
     * @param standard_deviation Standard_deviation of the normal
     * distribution.
     */
    double get_sample(double mean, double standard_deviation);

    /** @brief Obtain a sample of a normal distribution.
     *
     * Uses 0 as mean.
     * @returns sample
     * @param standard_deviation Standard_deviation of the normal
     * distribution.
     */

    double get_sample(double standard_deviation);

  private:
    pcg64 generator;
    std::normal_distribution<> dist;
  };

  /** \brief provides a truncated normal distribution sample on invocation.
   *
   * This uses a simple accept-reject algorithm. It should be highly
   * inefficient, if the probability of obtaining a sample of the usual normal
   * distribution within the interval [lower_bound,upper_bound] is small.
   * Also not thread safe, see under Normaldistribution.
   */
  class Truncatednormaldist final {
  public:
    Truncatednormaldist(
        std::array<uint32_t, pcg_seed_count> &used_seed,
        std::array<uint32_t, pcg_seed_count> seed = make_random_seed());

    /** @brief Obtain a sample of the truncated normal distribution.
     *
     * @returns sample with lower_bound <= sample <= upper_bound.
     * @param mean Mean of the UNtruncated normal distribution.
     * @param standard_deviation Standard_deviation of the UNtruncated normal
     * distribution.
     * @param lower_bound Lower bound of accepted samples.
     * @param upper_bound Upper bound of accepted samples.
     */
    double get_sample(
        double mean, double standard_deviation, double lower_bound,
        double upper_bound);

    /** @brief Obtain a sample of the truncated normal distribution.
     *
     * Uses mean +- 2 * standard_deviation as lower and upper bound.
     * @returns sample with lower_bound <= sample <= upper_bound.
     * @param mean Mean of the UNtruncated normal distribution.
     * @param standard_deviation Standard_deviation of the UNtruncated normal
     * distribution.
     */
    double get_sample(double mean, double standard_deviation);

    /** @brief Obtain a sample of the truncated normal distribution.
     *
     * Uses 0 as mean.
     * @returns sample with lower_bound <= sample <= upper_bound.
     * @param mean Mean of the UNtruncated normal distribution.
     * @param standard_deviation Standard_deviation of the UNtruncated normal
     * distribution.
     * @param lower_bound Lower bound of accepted samples.
     * @param upper_bound Upper bound of accepted samples.
     */

    double get_sample(
        double standard_deviation, double lower_bound, double upper_bound);

    /** @brief Obtain a sample of the truncated normal distribution.
     *
     * Uses 0 as mean.
     * Uses mean +- 2 * standard_deviation as lower and upper bound.
     * @returns sample with lower_bound <= sample <= upper_bound.
     * @param mean Mean of the UNtruncated normal distribution.
     * @param standard_deviation Standard_deviation of the UNtruncated normal
     * distribution.
     */

    double get_sample(double standard_deviation);

  private:
    Normaldistribution normaldist;
  };

} // namespace Aux
