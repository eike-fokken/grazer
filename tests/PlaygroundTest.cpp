#include <Eigen/Sparse>
#include <Eigen/src/SparseCore/SparseMatrix.h>
#include <Eigen/src/SparseCore/SparseUtil.h>
#include <Eigen/src/SparseLU/SparseLU.h>
#include <gtest/gtest.h>

TEST(SparseLU, use_in_vector) {

  using Solver = Eigen::SparseLU<Eigen::SparseMatrix<double>>;

  std::vector<Solver> vector(5);
  std::cout << "size: " << vector.size() << std::endl;
  std::cout << "capacity: " << vector.capacity() << std::endl;

  Eigen::SparseMatrix<double> A(2, 2);
  std::vector<Eigen::Triplet<double>> triplets;
  triplets.push_back({0, 0, 1.0});
  triplets.push_back({0, 1, 2.0});
  triplets.push_back({1, 0, 2.0});
  triplets.push_back({1, 1, 1.0});

  A.setFromTriplets(triplets.begin(), triplets.end());
  for (auto &solver : vector) { solver.compute(A); }

  double d = 0.0;
  for (auto &solver : vector) {
    Eigen::Vector2d b(1.0, d);
    d += 1.0;
    Eigen::Vector2d x = solver.solve(b);
    std::cout << x << "\n" << std::endl;
  }
}
