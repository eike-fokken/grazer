#pragma once
#include <Eigen/Sparse>
#include <nlohmann/json.hpp>
#include <Subproblem.hpp>

#include "gmock/gmock.h"


#include <string>


namespace GrazerTest {

  class MockSubproblem : public Model::Subproblem {

  public:
    MOCK_METHOD((std::string), get_type, (),
                (override, const));
    MOCK_METHOD(void, evaluate,
                ((Eigen::Ref<Eigen::VectorXd>),
                 (double), (double), (Eigen::Ref<Eigen::VectorXd const> const &),
                 (Eigen::Ref<Eigen::VectorXd const> const &)),
                (const, override));
    MOCK_METHOD(void, evaluate_state_derivative,
                ((Aux::Matrixhandler *), (double), (double),
                 (Eigen::Ref<Eigen::VectorXd const> const &last_state),
                 (Eigen::Ref<Eigen::VectorXd const> const &new_state)),
                (const,override));
    MOCK_METHOD(int, reserve_indices, (int const next_free_index), (override));
    MOCK_METHOD((void), set_initial_values,
                (Eigen::Ref<Eigen::VectorXd>, nlohmann::json), (override));
    MOCK_METHOD((void), save_values, (double, Eigen::Ref<Eigen::VectorXd>), (override));
    MOCK_METHOD((void), print_to_files, (std::filesystem::path const &),
                (override));
  };
} // namespace Model
