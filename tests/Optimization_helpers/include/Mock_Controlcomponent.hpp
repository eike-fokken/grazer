#include "Controlcomponent.hpp"
#include "Matrixhandler.hpp"
#include "Statecomponent.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using rootfunction
    = Eigen::VectorXd(Eigen::VectorXd, Eigen::VectorXd, Eigen::VectorXd);
using Derivative = Eigen::SparseMatrix<double>(
    Eigen::VectorXd, Eigen::VectorXd, Eigen::VectorXd);

class TestControlComponent_for_ControlStateCache final :
    public Model::Controlcomponent {
public:
  TestControlComponent_for_ControlStateCache(rootfunction _f, Derivative _df) :
      f(_f), df(_df) {}
  rootfunction *f;
  Derivative *df;

  MOCK_METHOD(void, setup, (), (final));

  void evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    rootvalues = f(
        last_state, new_state, control); // new_state - last_state - control;
  }

  void d_evalutate_d_new_state(
      Aux::Matrixhandler &jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    Eigen::SparseMatrix<double> mat = df(last_state, new_state, control);
    for (int k = 0; k < mat.outerSize(); ++k)
      for (Eigen::SparseMatrix<double>::InnerIterator it(mat, k); it; ++it) {
        jacobianhandler.set_coefficient(
            static_cast<int>(it.row()), static_cast<int>(it.col()), it.value());
      }
  }

  void prepare_timestep(
      double, double, Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &) final {
    std::cout << "Called prepare_timestep" << std::endl;
  };

  MOCK_METHOD(
      void, d_evalutate_d_last_state,
      ((Aux::Matrixhandler & jacobianhandler), (double last_time),
       (double new_time), (Eigen::Ref<Eigen::VectorXd const> const &last_state),
       (Eigen::Ref<Eigen::VectorXd const> const &new_state),
       (Eigen::Ref<Eigen::VectorXd const> const &control)),
      (const, final));

  MOCK_METHOD(
      void, d_evalutate_d_control,
      (Aux::Matrixhandler & jacobianhandler, (double last_time),
       (double new_time), (Eigen::Ref<Eigen::VectorXd const> const &last_state),
       (Eigen::Ref<Eigen::VectorXd const> const &new_state),
       (Eigen::Ref<Eigen::VectorXd const> const &control)),
      (const, final));

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

  // //////////////////////////////
  // // Statecomponent overrides:
  // /////////////////////////

  // Eigen::Index set_state_indices(Eigen::Index) final {
  //   state_startindex = 0;
  //   state_afterindex = 2;
  //   return state_afterindex;
  // }

  // MOCK_METHOD(void, add_results_to_json, (nlohmann::json &), (final));

  // MOCK_METHOD(
  //     void, json_save,
  //     ((double time), (Eigen::Ref<Eigen::VectorXd const> const &state)),
  //     (final));

  // // unneeded:
  // MOCK_METHOD(
  //     void, set_initial_values,
  //     ((Eigen::Ref<Eigen::VectorXd> new_state),
  //      (nlohmann::json const &initial_json)),
  //     (final));
};
