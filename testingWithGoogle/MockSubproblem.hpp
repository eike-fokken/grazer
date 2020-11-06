#include "gmock/gmock.h"
#include <Eigen/Sparse>
#include <Subproblem.hpp>

namespace Model {

class MockSubproblem : public Subproblem {

public:
  MOCK_METHOD(void, evaluate,
              ((Eigen::VectorXd &), (double), (double),
               (Eigen::VectorXd const &), (Eigen::VectorXd const &)),
              (override));
  MOCK_METHOD(void, evaluate_state_derivative,
              ((Eigen::SparseMatrix<double> &), (double), (double),
               (Eigen::VectorXd const &last_state),
               (Eigen::VectorXd const &new_state)),
              (override));
  MOCK_METHOD(unsigned int, reserve_indices,
              (unsigned int const next_free_index), (override));
};
} // namespace Model
