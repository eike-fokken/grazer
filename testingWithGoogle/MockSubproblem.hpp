#include <gmock/gmock.h>
#include "Subproblem.hpp"
#include <Eigen/Sparse>

namespace Model{

    class MockSubproblem : public Subproblem {

        public:
        MOCK_METHOD(void, evaluate, ((const Eigen::VectorXd &), (Eigen::VectorXd &)), (override));
        MOCK_METHOD(void, evaluate_state_derivative, ((const Eigen::VectorXd &), (Eigen::SparseMatrix<double> &)), (override));
        MOCK_METHOD(unsigned int, get_number_of_states,(),(override));

    };
}