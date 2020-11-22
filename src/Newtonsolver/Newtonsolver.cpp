#include "../../testingWithGoogle/TestProblem.hpp"
#include <Newtonsolver.hpp>
#include <Problem.hpp>

template class Solver::Newtonsolver_temp<Model::Problem>;
template class Solver::Newtonsolver_temp<TestProblem>;
