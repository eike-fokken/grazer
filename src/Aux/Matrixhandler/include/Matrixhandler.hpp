#pragma once
#include <Eigen/Sparse>
#include <vector>

namespace Aux {

  class Matrixhandler {

  public:
    Matrixhandler(Eigen::SparseMatrix<double> *_matrix) : matrix(_matrix){};

    virtual ~Matrixhandler(){};

    virtual void set_coefficient(int row, int col, double value) = 0;

    virtual void set_matrix() = 0;

  protected:
    Eigen::SparseMatrix<double> *matrix;
  };

  class Triplethandler final : public Matrixhandler {

  public:
    virtual ~Triplethandler() override{};

    virtual void set_coefficient(int row, int col, double value) override final;

    virtual void set_matrix() override final;

  private:
    std::vector<Eigen::Triplet<double>> tripletlist;
  };

  class Coeffrefhandler final : public Matrixhandler {

  public:
    virtual ~Coeffrefhandler() override{};

    virtual void set_coefficient(int row, int col, double value) override final;

    virtual void set_matrix() override final;
  };

} // namespace Aux
