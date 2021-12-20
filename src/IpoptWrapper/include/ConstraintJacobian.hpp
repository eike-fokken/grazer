#pragma once
#include "InterpolatingVector.hpp"
#include <Eigen/Dense>
#include <Eigen/src/Core/util/Meta.h>
#include <IpTypes.hpp>
#include <vector>

namespace Optimization {

  class ConstraintJacobian_Base {

  public:
    void base_supply_indices(
        Ipopt::Index *iRow, Ipopt::Index *jCol,
        Eigen::Index number_of_values) const;

    Eigen::Index nonZeros_of_structure() const;
    Eigen::Index get_outer_column_offset(Eigen::Index column) const;
    Eigen::Index get_inner_column_offset(Eigen::Index column) const;
    Eigen::Index get_block_size() const;
    Eigen::Index get_block_width() const;
    Eigen::Index get_block_height() const;

    Eigen::Index get_inner_col_height(Eigen::Index column) const;
    Eigen::Index get_outer_col_height(Eigen::Index column) const;
    Eigen::Index get_outer_width() const;
    Eigen::Index get_inner_width() const;
    Eigen::Index get_outer_height() const;
    Eigen::Index get_inner_height() const;

    Eigen::Index get_outer_rowstart_jacobian(Eigen::Index column) const;
    Eigen::Index get_inner_rowstart_jacobian(Eigen::Index column) const;

    Eigen::Index get_outer_colstart_jacobian(Eigen::Index column) const;
    Eigen::Index get_inner_colstart_jacobian(Eigen::Index column) const;

    Eigen::Index column_values_start(Eigen::Index column) const;
    Eigen::Index column_values_end(Eigen::Index column) const;

  protected:
    static ConstraintJacobian_Base make_instance(
        Aux::InterpolatingVector_Base const &constraints,
        Aux::InterpolatingVector_Base const &controls);

    Eigen::Ref<Eigen::MatrixXd>
    base_column_block(double *values, Eigen::Index column);
    Eigen::Ref<Eigen::MatrixXd const>
    base_column_block(double const *values, Eigen::Index column) const;

  private:
    ConstraintJacobian_Base(
        Eigen::Index block_width, Eigen::Index block_height,
        Eigen::Vector<Eigen::Index, Eigen::Dynamic> block_column_offsets);

    Eigen::Index const block_width;
    Eigen::Index const block_height;
    Eigen::Vector<Eigen::Index, Eigen::Dynamic> const block_column_offsets;
  };

  class MappedConstraintJacobian : public ConstraintJacobian_Base {
  public:
    MappedConstraintJacobian(
        double *values, Eigen::Index number_of_entries,
        Aux::InterpolatingVector_Base const &constraints,
        Aux::InterpolatingVector_Base const &controls);

    Eigen::Ref<Eigen::MatrixXd> get_nnz_column_block(Eigen::Index column);
    Eigen::Ref<Eigen::MatrixXd const>
    get_nnz_column_block(Eigen::Index column) const;
    Eigen::Index nonZeros() const;

    void setZero();

    void replace_storage(
        double *new_values, Eigen::Index length_must_be_equal_to_old_length);

    void supply_indices(
        Ipopt::Index *iRow, Ipopt::Index *jCol,
        Eigen::Index number_of_values) const;

    Eigen::MatrixXd whole_matrix() const;

  private:
    double *values;
    Eigen::Index number_of_entries;
  };

  class ConstraintJacobian : public ConstraintJacobian_Base {
  public:
    ConstraintJacobian(
        Aux::InterpolatingVector_Base const &constraints,
        Aux::InterpolatingVector_Base const &controls);

    Eigen::Ref<Eigen::MatrixXd> get_nnz_column_block(Eigen::Index column);
    Eigen::Ref<Eigen::MatrixXd const>
    get_nnz_column_block(Eigen::Index column) const;

    Eigen::Index nonZeros() const;
    void setZero();

    void supply_indices(
        Ipopt::Index *iRow, Ipopt::Index *jCol,
        Eigen::Index number_of_values) const;

    Eigen::MatrixXd whole_matrix() const;

  private:
    Eigen::Vector<double, Eigen::Dynamic> storage;
  };

} // namespace Optimization
