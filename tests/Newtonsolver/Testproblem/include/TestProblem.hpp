#pragma once
#include "Controlcomponent.hpp"
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <gmock/gmock.h>
#include <memory>
#include <vector>

namespace Aux {
  class Matrixhandler;
}
/*! \namespace Model
 *  This namespace holds all data for setting up model equations and for taking
 * derivatives thereof.
 */

using rootfunction = Eigen::VectorXd(Eigen::VectorXd);
using Derivative = Eigen::SparseMatrix<double>(Eigen::VectorXd);

class TestProblem final : public Model::Controlcomponent {

public:
  /// The constructor needs to declare Delta_t
  ///
  TestProblem(rootfunction _f, Derivative _df);

  void evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final;

  void d_evalutate_d_new_state(
      Aux::Matrixhandler &jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final;

  rootfunction *f;
  Derivative *df;

  MOCK_METHOD(void, setup, (), (final));

  MOCK_METHOD(
      void, d_evalutate_d_last_state,
      ((Aux::Matrixhandler &), (double), (double),
       (Eigen::Ref<Eigen::VectorXd const> const &),
       (Eigen::Ref<Eigen::VectorXd const> const &),
       (Eigen::Ref<Eigen::VectorXd const> const &)),
      (const, final));

  MOCK_METHOD(
      void, d_evalutate_d_control,
      ((Aux::Matrixhandler & jacobianhandler), (double last_time),
       (double new_time), (Eigen::Ref<Eigen::VectorXd const> const &last_state),
       (Eigen::Ref<Eigen::VectorXd const> const &new_state),
       (Eigen::Ref<Eigen::VectorXd const> const &control)),
      (const, final));

  MOCK_METHOD(
      Eigen::Index, set_control_indices, (Eigen::Index next_free_index),
      (final));

  MOCK_METHOD(
      void, set_initial_controls,
      ((Aux::InterpolatingVector_Base & full_control_vector),
       (nlohmann::json const &control_json)),
      (const, final));

  MOCK_METHOD(
      void, set_lower_bounds,
      ((Aux::InterpolatingVector_Base & full_control_vector),
       (nlohmann::json const &control_json)),
      (const, final));

  MOCK_METHOD(
      void, set_upper_bounds,
      ((Aux::InterpolatingVector_Base & full_control_vector),
       (nlohmann::json const &control_json)),
      (const, final));

private:
  std::string componentclass() final {
    assert(false);
    return std::string();
  }
  std::string componenttype() final {
    assert(false);
    return std::string();
  }
  std::string id() final {
    assert(false);
    return std::string();
  }
};
