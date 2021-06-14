#include "Problem.hpp"
#include "MockSubproblem.hpp"
#include "test_io_helper.hpp"
#include <gtest/gtest.h>

class modelTest : public CWD_To_Rand_Test_Dir {};
TEST_F(modelTest, get_number_of_states) {

  // Test how often this class is being called
  GrazerTest::MockSubproblem mocksub;
  EXPECT_CALL(mocksub, reserve_indices(0)).Times(1);

  mocksub.set_indices(0);
}

TEST_F(modelTest, Model_evaluate) {

  auto problem_json = R"( {"subproblems": {}} )"_json;
  auto output_dir = std::filesystem::path("");
  Model::Problem problem(problem_json, output_dir);

  // make unique pointer of mocksub1 and mocksub2
  auto mock1_ptr = std::make_unique<GrazerTest::MockSubproblem>();
  auto mock2_ptr = std::make_unique<GrazerTest::MockSubproblem>();

  // add subproblem to problem
  problem.add_subproblem(std::move(mock1_ptr));
  problem.add_subproblem(std::move(mock2_ptr));

  // call problem.evaluate
  double last_time(0.0);
  double new_time(1.0);
  Eigen::VectorXd rootvalues(2);
  Eigen::VectorXd v1(2);
  Eigen::VectorXd v2(2);

  // This is necessary for the expect_call to work properly...
  Eigen::Ref<Eigen::VectorXd> rootref(rootvalues);

  // expect the call to evaluate on the subproblems.
  // The cast magic is necessary to have the right type at hand...
  EXPECT_CALL(
      *dynamic_cast<GrazerTest::MockSubproblem *>(problem.get_subproblems()[0]),
      evaluate(
          Eigen::Ref<Eigen::VectorXd>(rootvalues), last_time, new_time,
          Eigen::Ref<Eigen::VectorXd const>(v1),
          Eigen::Ref<Eigen::VectorXd const>(v2)))
      .Times(1);
  EXPECT_CALL(
      *dynamic_cast<GrazerTest::MockSubproblem *>(problem.get_subproblems()[1]),
      evaluate(
          Eigen::Ref<Eigen::VectorXd>(rootvalues), last_time, new_time,
          Eigen::Ref<Eigen::VectorXd const>(v1),
          Eigen::Ref<Eigen::VectorXd const>(v2)))
      .Times(1);

  problem.evaluate(rootvalues, last_time, new_time, v1, v2);
}
