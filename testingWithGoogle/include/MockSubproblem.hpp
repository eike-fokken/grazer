#pragma once
#include "gmock/gmock.h"
#include <Eigen/Sparse>
#include <Matrixhandler.hpp>
#include <Subproblem.hpp>

namespace GrazerTest {

  class MockSubproblem : public Model::Subproblem {

  public:
    MOCK_METHOD(void, evaluate,
                ((Eigen::VectorXd &), (double), (double),
                 (Eigen::VectorXd const &), (Eigen::VectorXd const &)),
                (override));
    MOCK_METHOD(void, evaluate_state_derivative,
                ((Aux::Matrixhandler *), (double), (double),
                 (Eigen::VectorXd const &last_state),
                 (Eigen::VectorXd const &new_state)),
                (override));
    MOCK_METHOD(int, reserve_indices, (int const next_free_index), (override));
    MOCK_METHOD(void, display, (), (override, const));
    MOCK_METHOD((void), set_initial_values,
                (Eigen::VectorXd &, nlohmann::ordered_json), (override));
    MOCK_METHOD((void), save_values, (double, Eigen::VectorXd &), (override));
    MOCK_METHOD((void), print_to_files, (std::filesystem::path const &),
                (override));
  };
} // namespace Model
