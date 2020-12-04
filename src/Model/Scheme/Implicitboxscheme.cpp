#include <Implicitboxscheme.hpp>

namespace Model::Scheme {
  
  

    /// Computes the implicit box scheme at one point.
    Eigen::Vector2d Implicitboxscheme::evaluate_point(
        double last_time, double new_time, Eigen::Vector2d const &last_u_jm1,
        Eigen::Vector2d const &last_u_j, Eigen::Vector2d const &new_u_jm1,
        Eigen::Vector2d const &new_u_j) const {

    }

    /// The derivative with respect to \code{.cpp}last_u_jm1\endcode
    Eigen::Matrix2d Implicitboxscheme::devaluate_point_dleft(
        double last_time, double new_time, Eigen::Vector2d const &last_u_jm1,
        Eigen::Vector2d const &last_u_j, Eigen::Vector2d const &new_u_jm1,
        Eigen::Vector2d const &new_u_j) const {

    }

    /// The derivative with respect to \code{.cpp}last_u_j\endcode
    Eigen::Matrix2d Implicitboxscheme::devaluate_point_dright(
        double last_time, double new_time, Eigen::Vector2d const &last_u_jm1,
        Eigen::Vector2d const &last_u_j, Eigen::Vector2d const &new_u_jm1,
        Eigen::Vector2d const &new_u_j) const {

    }

} // namespace Model::Scheme

