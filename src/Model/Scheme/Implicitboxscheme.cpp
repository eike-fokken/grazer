#include <Implicitboxscheme.hpp>

namespace Model::Scheme {
  
  

    /// Computes the implicit box scheme at one point.
    void Implicitboxscheme::evaluate_point(
        Eigen::Ref<Eigen::Vector2d> result, double last_time, double new_time,
        double Delta_x, Eigen::Vector2d const &last_u_jm1,
        Eigen::Vector2d const &last_u_j, Eigen::Vector2d const &new_u_jm1,
        Eigen::Vector2d const &new_u_j) const {
      double Delta_t = new_time - last_time;
      result = 0.5*(new_u_jm1 + new_u_jm1)  - 0.5*(last_u_jm1 + last_u_j)  + Delta_t/Delta_x * ( bl.flux(new_u_j) - bl.flux(new_u_jm1)) +0.5*Delta_t * (bl.source(new_u_j)+bl.source(new_u_jm1));
    }

    /// The derivative with respect to \code{.cpp}last_u_jm1\endcode
    Eigen::Matrix2d Implicitboxscheme::devaluate_point_dleft(double last_time, double new_time, double Delta_x, Eigen::Vector2d const &,
        Eigen::Vector2d const &, Eigen::Vector2d const &new_u_jm1,
        Eigen::Vector2d const &) const {
      double Delta_t = new_time-last_time;
      Eigen::Matrix2d jac;
      Eigen::Matrix2d id;
      id.setIdentity();
      jac = 0.5*id -Delta_t/Delta_x*bl.dflux_dstate(new_u_jm1)- 0.5*Delta_t*bl.dsource_dstate(new_u_jm1);
      return jac;
    }

    /// The derivative with respect to \code{.cpp}last_u_j\endcode
    Eigen::Matrix2d Implicitboxscheme::devaluate_point_dright(
        double last_time, double new_time, double Delta_x,
        Eigen::Vector2d const &, Eigen::Vector2d const &,
        Eigen::Vector2d const &,
        Eigen::Vector2d const &new_u_j) const {
      double Delta_t = new_time - last_time;
      Eigen::Matrix2d jac;
      Eigen::Matrix2d id;
      id.setIdentity();
      jac = 0.5 * id + Delta_t / Delta_x * bl.dflux_dstate(new_u_j) -
            0.5 *Delta_t* bl.dsource_dstate(new_u_j);
      return jac;
    }

} // namespace Model::Scheme

