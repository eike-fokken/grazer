#pragma once
#include <Eigen/Sparse>
#include <Isothermaleulerequation.hpp>


namespace Model::Scheme {
  
  class Implicitboxscheme {
  public:
    
    /// Computes the implicit box scheme at one point.
    void evaluate_point(Eigen::Ref<Eigen::Vector2d> result, double last_time,
                        double new_time, double Delta_x,
                        Eigen::Ref<Eigen::Vector2d const> const &last_u_jm1,
                        Eigen::Ref<Eigen::Vector2d const> const &last_u_j,
                        Eigen::Ref<Eigen::Vector2d const> const &new_u_jm1,
                        Eigen::Ref<Eigen::Vector2d const> const &new_u_j,
                        Balancelaw::Isothermaleulerequation bl) const;

    /// The derivative with respect to \code{.cpp}last_u_jm1\endcode
    Eigen::Matrix2d devaluate_point_dleft(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector2d const> const &last_u_jm1, Eigen::Ref<Eigen::Vector2d const> const &last_u_j,
        Eigen::Ref<Eigen::Vector2d const> const &new_u_jm1, Eigen::Ref<Eigen::Vector2d const> const &new_u_j,
        Balancelaw::Isothermaleulerequation bl) const;

    /// The derivative with respect to \code{.cpp}last_u_j\endcode
    Eigen::Matrix2d devaluate_point_dright(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector2d const> const &last_u_jm1, Eigen::Ref<Eigen::Vector2d const> const &last_u_j,
        Eigen::Ref<Eigen::Vector2d const> const &new_u_jm1, Eigen::Ref<Eigen::Vector2d const> const &new_u_j,
        Balancelaw::Isothermaleulerequation bl) const;
  };


}
