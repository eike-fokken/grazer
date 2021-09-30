#pragma once
#include "Threepointscheme.hpp"
#include <Eigen/Sparse>

namespace Model::Balancelaw {
  class Pipe_Balancelaw;
} // namespace Model::Balancelaw

namespace Model::Scheme {

  class Implicitboxscheme : public Threepointscheme {
  public:
    ~Implicitboxscheme() override {}
    /// Computes the implicit box scheme at one point.
    void evaluate_point(
        Eigen::Ref<Eigen::Vector2d> result, double last_time, double new_time,
        double Delta_x, Eigen::Ref<Eigen::Vector2d const> last_left,
        Eigen::Ref<Eigen::Vector2d const> last_right,
        Eigen::Ref<Eigen::Vector2d const> new_left,
        Eigen::Ref<Eigen::Vector2d const> new_right,
        Model::Balancelaw::Pipe_Balancelaw const &bl) const override;

    /// The derivative with respect to \code{.cpp}last_left\endcode
    Eigen::Matrix2d devaluate_point_d_new_left(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector2d const> last_left,
        Eigen::Ref<Eigen::Vector2d const> last_right,
        Eigen::Ref<Eigen::Vector2d const> new_left,
        Eigen::Ref<Eigen::Vector2d const> new_right,
        Model::Balancelaw::Pipe_Balancelaw const &bl) const override;

    /// \brief The derivative with respect to \code{.cpp}last_right\endcode
    Eigen::Matrix2d devaluate_point_d_new_right(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector2d const> last_left,
        Eigen::Ref<Eigen::Vector2d const> last_right,
        Eigen::Ref<Eigen::Vector2d const> new_left,
        Eigen::Ref<Eigen::Vector2d const> new_right,
        Model::Balancelaw::Pipe_Balancelaw const &bl) const override;
  };
} // namespace Model::Scheme
