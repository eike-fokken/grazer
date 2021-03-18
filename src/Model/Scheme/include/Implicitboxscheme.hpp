#pragma once
#include <Eigen/Sparse>

namespace Model::Balancelaw {
  class Isothermaleulerequation;
}


namespace Model::Scheme {
  
  class Implicitboxscheme {
  public:
    
    /// Computes the implicit box scheme at one point.
    void evaluate_point(Eigen::Ref<Eigen::Vector2d> result, double last_time,
                        double new_time, double Delta_x,
                        Eigen::Ref<Eigen::Vector2d const> const &last_left,
                        Eigen::Ref<Eigen::Vector2d const> const &last_right,
                        Eigen::Ref<Eigen::Vector2d const> const &new_left,
                        Eigen::Ref<Eigen::Vector2d const> const &new_right,
                        Model::Balancelaw::Isothermaleulerequation const &bl,double diameter,double roughness) const;

    /// The derivative with respect to \code{.cpp}last_left\endcode
    Eigen::Matrix2d devaluate_point_dleft(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector2d const> const &last_left,
        Eigen::Ref<Eigen::Vector2d const> const &last_right,
        Eigen::Ref<Eigen::Vector2d const> const &new_left,
        Eigen::Ref<Eigen::Vector2d const> const &new_right, Model::Balancelaw::Isothermaleulerequation const &bl,double diameter,double roughness) const;

    /// \brief The derivative with respect to \code{.cpp}last_right\endcode
    Eigen::Matrix2d devaluate_point_dright(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector2d const> const &last_left,
        Eigen::Ref<Eigen::Vector2d const> const &last_right,
        Eigen::Ref<Eigen::Vector2d const> const &new_left,
        Eigen::Ref<Eigen::Vector2d const> const &new_right, Model::Balancelaw::Isothermaleulerequation const &bl,double diameter,double roughness)
        const;
  };
}
