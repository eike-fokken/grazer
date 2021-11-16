#include "Controlcomponent.hpp"
#include "Matrixhandler.hpp"
#include "Statecomponent.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

class TestControlComponent_for_ControlStateCache final :
    public Model::Controlcomponent {

  MOCK_METHOD(void, setup, (), (final));

  void evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    rootvalues = new_state - last_state - control;
  }

  void d_evalutate_d_new_state(
      Aux::Matrixhandler &jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &) const final {
    jacobianhandler.set_coefficient(0, 0, 1.0);
    jacobianhandler.set_coefficient(1, 1, 1.0);
  }

  MOCK_METHOD(
      void, prepare_timestep,
      ((double last_time), (double new_time),
       (Eigen::Ref<Eigen::VectorXd const> const &last_state),
       (Eigen::Ref<Eigen::VectorXd const> const &new_state),
       (Eigen::Ref<Eigen::VectorXd const> const &control)),
      (final));

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
