#pragma once
#include "InterpolatingVector.hpp"
#include <Eigen/Dense>
#include <IpTypes.hpp>
#include <vector>

namespace Optimization {

  class ConstraintJacobian_impl;

  class ConstraintJacobian_impl {
  public:
    static ConstraintJacobian_impl make_instance(
        Aux::InterpolatingVector_Base const &constraints,
        Aux::InterpolatingVector_Base const &controls);

    Eigen::Ref<Eigen::MatrixXd> get_nnz_column_block(
        double *values, Eigen::Index number_of_entries, Eigen::Index column);
    Eigen::Ref<Eigen::MatrixXd const> get_nnz_column_block(
        double *values, Eigen::Index number_of_entries,
        Eigen::Index column) const;

    Eigen::Index nonZeros_of_structure() const;
    void setZero(double *values, Eigen::Index number_of_entries);

  private:
    ConstraintJacobian_impl(
        Eigen::Index block_width,
        Eigen::Vector<Eigen::Index, Eigen::Dynamic> block_column_offsets);

    Eigen::Index const block_width;
    Eigen::Vector<Eigen::Index, Eigen::Dynamic> const block_column_offsets;
  };

  class MappedConstraintJacobian {
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

  private:
    double *values;
    Eigen::Index number_of_entries;
    ConstraintJacobian_impl impl;
  };

  class ConstraintJacobian {
  public:
    ConstraintJacobian(
        Aux::InterpolatingVector_Base const &constraints,
        Aux::InterpolatingVector_Base const &controls);

  private:
    Eigen::Ref<Eigen::MatrixXd> get_nnz_column_block(Eigen::Index column);
    Eigen::Ref<Eigen::MatrixXd const>
    get_nnz_column_block(Eigen::Index column) const;
    Eigen::Index nonZeros() const;
    void setZero();

    Eigen::Vector<double, Eigen::Dynamic> storage;
    ConstraintJacobian_impl impl;
  };

} // namespace Optimization
