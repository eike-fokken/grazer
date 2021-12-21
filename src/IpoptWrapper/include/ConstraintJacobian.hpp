#pragma once
#include "InterpolatingVector.hpp"
#include <Eigen/Dense>
#include <IpTypes.hpp>
#include <tuple>
#include <vector>

namespace Optimization {

  std::tuple<Eigen::Index, Eigen::Index, Eigen::VectorX<Eigen::Index>>
  make_data(
      Aux::InterpolatingVector_Base const &constraints,
      Aux::InterpolatingVector_Base const &controls);

  class ConstraintJacobian;

  class ConstraintJacobian_Base {

  public:
    ConstraintJacobian_Base(
        std::tuple<Eigen::Index, Eigen::Index, Eigen::VectorX<Eigen::Index>>
            height_width_offsets);

    ConstraintJacobian_Base(
        Eigen::Index block_width, Eigen::Index block_height,
        Eigen::Vector<Eigen::Index, Eigen::Dynamic> block_column_offsets);

    virtual ~ConstraintJacobian_Base() = default;

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

    void setZero();

    Eigen::Ref<Eigen::MatrixXd> get_column_block(Eigen::Index column);
    Eigen::Ref<Eigen::MatrixXd const>
    get_column_block(Eigen::Index column) const;
    Eigen::Index nonZeros() const;

    void supply_indices(
        Ipopt::Index *iRow, Ipopt::Index *jCol,
        Eigen::Index number_of_values) const;
    Eigen::MatrixXd whole_matrix() const;

  private:
    Eigen::Ref<Eigen::VectorXd> underlying_storage();
    Eigen::Ref<Eigen::VectorXd const> underlying_storage() const;

    virtual double *get_value_pointer() = 0;
    virtual double const *get_value_pointer() const = 0;

    Eigen::Index const block_height;
    Eigen::Index const block_width;
    Eigen::Vector<Eigen::Index, Eigen::Dynamic> const block_column_offsets;

  protected:
    void assignment_helper(ConstraintJacobian_Base const &other);
  };

  class MappedConstraintJacobian : public ConstraintJacobian_Base {
  public:
    MappedConstraintJacobian(
        double *values, Eigen::Index number_of_entries,
        Aux::InterpolatingVector_Base const &constraints,
        Aux::InterpolatingVector_Base const &controls);

    void replace_storage(
        double *new_values, Eigen::Index length_must_be_equal_to_old_length);

    MappedConstraintJacobian &operator=(MappedConstraintJacobian const &);
    MappedConstraintJacobian &operator=(ConstraintJacobian const &);

  private:
    double *values;
    Eigen::Index number_of_entries;

    double *get_value_pointer() final;
    double const *get_value_pointer() const final;
  };

  class ConstraintJacobian : public ConstraintJacobian_Base {
  public:
    ConstraintJacobian(
        Aux::InterpolatingVector_Base const &constraints,
        Aux::InterpolatingVector_Base const &controls);

    ConstraintJacobian &operator=(MappedConstraintJacobian const &);
    ConstraintJacobian &operator=(ConstraintJacobian const &);

  private:
    Eigen::Vector<double, Eigen::Dynamic> storage;

    double *get_value_pointer() final;
    double const *get_value_pointer() const final;
  };

} // namespace Optimization
