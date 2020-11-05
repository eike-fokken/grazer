#include "gmock/gmock.h"
#include <Eigen/Sparse>
#include <Subproblem.hpp>


namespace Model {

class MockSubproblem : public Subproblem {

public:
  MOCK_METHOD(void, evaluate,
              ((double), (double), (const Eigen::VectorXd &),
               (Eigen::VectorXd &)),
              (override));
  MOCK_METHOD(void, evaluate_state_derivative,
              ((double),(double),(const Eigen::VectorXd &), (Eigen::SparseMatrix<double> &)),
              (override));
  MOCK_METHOD(unsigned int, reserve_indices,
              (unsigned int const next_free_index), (override));
};
} // namespace Model
