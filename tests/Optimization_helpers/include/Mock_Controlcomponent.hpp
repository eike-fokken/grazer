#include "Controlcomponent.hpp"
#include "Matrixhandler.hpp"
#include "Statecomponent.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <limits>

using rootfunction = Eigen::VectorXd(
    Eigen::Ref<Eigen::VectorXd const> const &,
    Eigen::Ref<Eigen::VectorXd const> const &,
    Eigen::Ref<Eigen::VectorXd const> const &);
using Derivative = Eigen::SparseMatrix<double>(
    Eigen::Ref<Eigen::VectorXd const> const &,
    Eigen::Ref<Eigen::VectorXd const> const &,
    Eigen::Ref<Eigen::VectorXd const> const &);

class TestControlComponent_for_ControlStateCache final :
    public Model::Controlcomponent {
public:
  TestControlComponent_for_ControlStateCache(
      rootfunction _f, Derivative _df_dnew_state, Derivative _df_dlast_state,
      Derivative _df_dcontrol) :
      f(_f),
      df_dnew_state(_df_dnew_state),
      df_dlast_state(_df_dlast_state),
      df_dcontrol(_df_dcontrol) {}
  rootfunction *f;
  Derivative *df_dnew_state;
  Derivative *df_dlast_state;
  Derivative *df_dcontrol;

  MOCK_METHOD(Eigen::Index, get_control_startindex, (), (const, final));
  MOCK_METHOD(Eigen::Index, get_control_afterindex, (), (const, final));

  MOCK_METHOD(void, setup, (), (final));

  void evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    rootvalues = f(
        last_state, new_state, control); // new_state - last_state - control;
  }

  void d_evaluate_d_new_state(
      Aux::Matrixhandler &jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    Eigen::SparseMatrix<double> mat
        = df_dnew_state(last_state, new_state, control);
    for (int k = 0; k < mat.outerSize(); ++k)
      for (Eigen::SparseMatrix<double>::InnerIterator it(mat, k); it; ++it) {
        jacobianhandler.add_to_coefficient(
            static_cast<int>(it.row()), static_cast<int>(it.col()), it.value());
      }
  }

  void d_evaluate_d_last_state(
      Aux::Matrixhandler &jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {

    Eigen::SparseMatrix<double> mat
        = df_dlast_state(last_state, new_state, control);
    for (int k = 0; k < mat.outerSize(); ++k)
      for (Eigen::SparseMatrix<double>::InnerIterator it(mat, k); it; ++it) {
        jacobianhandler.add_to_coefficient(
            static_cast<int>(it.row()), static_cast<int>(it.col()), it.value());
      }
  }

  void d_evaluate_d_control(
      Aux::Matrixhandler &jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {

    Eigen::SparseMatrix<double> mat
        = df_dcontrol(last_state, new_state, control);
    for (int k = 0; k < mat.outerSize(); ++k)
      for (Eigen::SparseMatrix<double>::InnerIterator it(mat, k); it; ++it) {
        jacobianhandler.add_to_coefficient(
            static_cast<int>(it.row()), static_cast<int>(it.col()), it.value());
      }
  }

  void prepare_timestep(
      double, double, Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &) final {
    std::cout << "Called prepare_timestep" << std::endl;
  };

  MOCK_METHOD(Eigen::Index, set_control_indices, (Eigen::Index), (final));

  // not needed:
  MOCK_METHOD(
      void, set_initial_controls,
      ((Aux::InterpolatingVector_Base &), (nlohmann::json const &)),
      (const, final));

  MOCK_METHOD(
      void, set_lower_bounds,
      ((Aux::InterpolatingVector_Base &), (nlohmann::json const &)),
      (const, final));

  MOCK_METHOD(
      void, set_upper_bounds,
      ((Aux::InterpolatingVector_Base &), (nlohmann::json const &)),
      (const, final));

  std::string componentclass() const final {
    assert(false);
    return std::string();
  }
  std::string componenttype() const final {
    assert(false);
    return std::string();
  }
  std::string id() const final {
    assert(false);
    return std::string();
  }
};
