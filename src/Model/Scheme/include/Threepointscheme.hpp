#pragma once
#include <Eigen/Sparse>

namespace Model::Balancelaw {
  class Pipe_Balancelaw;
} // namespace Model::Balancelaw

namespace Model::Scheme {

  class Threepointscheme {

  public:
    virtual ~Threepointscheme() {}

    /// Computes the scheme at one point.
    virtual void evaluate_point(
        Eigen::Ref<Eigen::Vector2d> result, double last_time, double new_time,
        double Delta_x, Eigen::Ref<Eigen::Vector2d const> last_left,
        Eigen::Ref<Eigen::Vector2d const> last_right,
        Eigen::Ref<Eigen::Vector2d const> new_left,
        Eigen::Ref<Eigen::Vector2d const> new_right,
        Model::Balancelaw::Pipe_Balancelaw const &bl) const = 0;

    /// The derivative with respect to \code{.cpp}new_left\endcode
    virtual Eigen::Matrix2d devaluate_point_d_new_left(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector2d const> last_left,
        Eigen::Ref<Eigen::Vector2d const> last_right,
        Eigen::Ref<Eigen::Vector2d const> new_left,
        Eigen::Ref<Eigen::Vector2d const> new_right,
        Model::Balancelaw::Pipe_Balancelaw const &bl) const = 0;

    /// \brief The derivative with respect to \code{.cpp}new_right\endcode
    virtual Eigen::Matrix2d devaluate_point_d_new_right(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector2d const> last_left,
        Eigen::Ref<Eigen::Vector2d const> last_right,
        Eigen::Ref<Eigen::Vector2d const> new_left,
        Eigen::Ref<Eigen::Vector2d const> new_right,
        Model::Balancelaw::Pipe_Balancelaw const &bl) const = 0;

    /// The derivative with respect to \code{.cpp}last_left\endcode
    virtual Eigen::Matrix2d devaluate_point_d_last_left(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector2d const> last_left,
        Eigen::Ref<Eigen::Vector2d const> last_right,
        Eigen::Ref<Eigen::Vector2d const> new_left,
        Eigen::Ref<Eigen::Vector2d const> new_right,
        Model::Balancelaw::Pipe_Balancelaw const &bl) const = 0;

    /// \brief The derivative with respect to \code{.cpp}last_right\endcode
    virtual Eigen::Matrix2d devaluate_point_d_last_right(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector2d const> last_left,
        Eigen::Ref<Eigen::Vector2d const> last_right,
        Eigen::Ref<Eigen::Vector2d const> new_left,
        Eigen::Ref<Eigen::Vector2d const> new_right,
        Model::Balancelaw::Pipe_Balancelaw const &bl) const = 0;
  };
} // namespace Model::Scheme
