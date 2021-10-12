#pragma once
#include <Eigen/Sparse>
#include <vector>

namespace Aux {

  /// \brief A utility base class that is used to setup and modify a sparse
  /// matrix.
  class Matrixhandler {

  public:
    Matrixhandler() = delete;
    Matrixhandler(Eigen::SparseMatrix<double> &_matrix) : matrix(_matrix){};

    virtual ~Matrixhandler();

    /// \brief Adds to a coefficient.
    ///
    /// @param row The row index of the coefficient.
    /// @param col The column index of the coefficient.
    /// @param value The value to be added to the already present coefficient.
    virtual void add_to_coefficient(int row, int col, double value) = 0;

    /// \brief Sets a coefficient. For #Triplethandler this actually behaves
    /// like #add_to_coefficient.
    ///
    /// @param row The row index of the coefficient.
    /// @param col The column index of the coefficient.
    /// @param value The value to be inserted.
    virtual void set_coefficient(int row, int col, double value) = 0;

    /// For #Triplethandler: Builds the matrix from the gathered coefficients
    /// and then forgets the coefficients.
    ///
    /// For #Coeffrefhandler: Does nothing.
    virtual void set_matrix() = 0;

  protected:
    Eigen::SparseMatrix<double> &matrix;
  };

  /// \brief The Triplethandler variety gathers the coefficients in triplets and
  /// later builds the matrix from the triplets.
  class Triplethandler final : public Matrixhandler {

  public:
    using Matrixhandler::Matrixhandler;
    ~Triplethandler() override{};

    void add_to_coefficient(int row, int col, double value) final;
    void set_coefficient(int row, int col, double value) final;

    void set_matrix() final;

  private:
    std::vector<Eigen::Triplet<double>> tripletlist;
  };

  /// \brief The Coeffrefhandler variety directly sets the coefficients and
  /// contains no state.
  class Coeffrefhandler final : public Matrixhandler {

  public:
    using Matrixhandler::Matrixhandler;

    ~Coeffrefhandler() override{};

    void add_to_coefficient(int row, int col, double value) final;
    void set_coefficient(int row, int col, double value) final;
    void set_matrix() final;
  };

} // namespace Aux
