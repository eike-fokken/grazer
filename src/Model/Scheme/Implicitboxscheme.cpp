#include <Implicitboxscheme.hpp>
#include <Isothermaleulerequation.hpp>

namespace Model::Scheme {

  /// Computes the implicit box scheme at one point.
  void Implicitboxscheme::evaluate_point(
      Eigen::Ref<Eigen::Vector2d> result, double last_time, double new_time,
      double Delta_x, Eigen::Ref<Eigen::Vector2d const> const &last_left,
      Eigen::Ref<Eigen::Vector2d const> const &last_right,
      Eigen::Ref<Eigen::Vector2d const> const &new_left,
      Eigen::Ref<Eigen::Vector2d const> const &new_right,
      Model::Balancelaw::Isothermaleulerequation const &bl, double diameter,
      double roughness) {

    double Delta_t = new_time - last_time;
    result
        = 0.5 * (new_left + new_right) - 0.5 * (last_left + last_right)
          - Delta_t / Delta_x
                * (bl.flux(new_left, diameter) - bl.flux(new_right, diameter))
          - 0.5 * Delta_t
                * (bl.source(new_right, diameter, roughness)
                   + bl.source(new_left, diameter, roughness));
  }

  /// The derivative with respect to \code{.cpp}last_left\endcode
  Eigen::Matrix2d Implicitboxscheme::devaluate_point_dleft(
      double last_time, double new_time, double Delta_x,
      Eigen::Ref<Eigen::Vector2d const> const &,
      Eigen::Ref<Eigen::Vector2d const> const &,
      Eigen::Ref<Eigen::Vector2d const> const &new_left,
      Eigen::Ref<Eigen::Vector2d const> const &,
      Model::Balancelaw::Isothermaleulerequation const &bl, double diameter,
      double roughness) {
    double Delta_t = new_time - last_time;
    Eigen::Matrix2d jac;
    Eigen::Matrix2d id;
    id.setIdentity();
    jac = 0.5 * id - Delta_t / Delta_x * bl.dflux_dstate(new_left, diameter)
          - 0.5 * Delta_t * bl.dsource_dstate(new_left, diameter, roughness);
    return jac;
  }

  /// The derivative with respect to \code{.cpp}last_right\endcode
  Eigen::Matrix2d Implicitboxscheme::devaluate_point_dright(
      double last_time, double new_time, double Delta_x,
      Eigen::Ref<Eigen::Vector2d const> const &,
      Eigen::Ref<Eigen::Vector2d const> const &,
      Eigen::Ref<Eigen::Vector2d const> const &,
      Eigen::Ref<Eigen::Vector2d const> const &new_right,
      Model::Balancelaw::Isothermaleulerequation const &bl, double diameter,
      double roughness) {
    double Delta_t = new_time - last_time;
    Eigen::Matrix2d jac;
    Eigen::Matrix2d id;
    id.setIdentity();
    jac = 0.5 * id + Delta_t / Delta_x * bl.dflux_dstate(new_right, diameter)
          - 0.5 * Delta_t * bl.dsource_dstate(new_right, diameter, roughness);
    return jac;
  }

} // namespace Model::Scheme
