#pragma once
#include "Subproblem.hpp"
#include "gmock/gmock.h"
#include <Eigen/Sparse>
#include <nlohmann/json.hpp>
#include <string>

namespace GrazerTest {

  class MockSubproblem : public Model::Subproblem {

  public:
    MOCK_METHOD((std::string), get_type, (), (override, const));
    MOCK_METHOD(
        void, evaluate,
        ((Eigen::Ref<Eigen::VectorXd>), (double), (double),
         (Eigen::Ref<Eigen::VectorXd const> const &),
         (Eigen::Ref<Eigen::VectorXd const> const &),
         (Eigen::Ref<Eigen::VectorXd const> const &),
         (Eigen::Ref<Eigen::VectorXd const> const &)),
        (const, override));
    MOCK_METHOD(
        void, prepare_timestep,
        ((double), (double), (Eigen::Ref<Eigen::VectorXd const> const &),
         (Eigen::Ref<Eigen::VectorXd const> const &),
         (Eigen::Ref<Eigen::VectorXd const> const &),
         (Eigen::Ref<Eigen::VectorXd const> const &)),
        (override));

    MOCK_METHOD(
        void, d_evalutate_d_new_state,
        ((Aux::Matrixhandler &), (double), (double),
         (Eigen::Ref<Eigen::VectorXd const> const &),
         (Eigen::Ref<Eigen::VectorXd const> const &),
         (Eigen::Ref<Eigen::VectorXd const> const &),
         (Eigen::Ref<Eigen::VectorXd const> const &)),
        (const, override));
    MOCK_METHOD(
        void, d_evalutate_d_last_state,
        ((Aux::Matrixhandler &), (double), (double),
         (Eigen::Ref<Eigen::VectorXd const> const &),
         (Eigen::Ref<Eigen::VectorXd const> const &),
         (Eigen::Ref<Eigen::VectorXd const> const &),
         (Eigen::Ref<Eigen::VectorXd const> const &)),
        (const, override));

    MOCK_METHOD(
        int, reserve_state_indices, (int const next_free_index), (override));
    MOCK_METHOD(
        (void), set_initial_values,
        (Eigen::Ref<Eigen::VectorXd>, nlohmann::json), (override));
    MOCK_METHOD(
        (void), json_save, (double, Eigen::Ref<Eigen::VectorXd const> const &),
        (override));
    MOCK_METHOD((void), add_results_to_json, (nlohmann::json &), (override));
  };
} // namespace GrazerTest
