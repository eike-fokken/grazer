#pragma once
namespace Aux {

  extern double const Pi;
  /**< The mathematical constant Pi */

  extern double const EPSILON; /**< This constant is chosen such that doubles
  whose differences are less than EPSILON, are considered equal.  */

  double circle_area(double radius);

  double smooth_abs(double x); /**< A C^1 absolute value function. */

  double dsmooth_abs_dx(double x); /**< The derivative of smooth_abs. */

  int dabs_dx(double x); /**< This is a derivative function for double
                          * std::abs(double x). It throws an exception, if it is
                          * called on a value smaller than 1e-13. */

} // namespace Aux
