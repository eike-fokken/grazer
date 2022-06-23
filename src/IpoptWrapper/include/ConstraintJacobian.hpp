/*
 * Grazer - network simulation and optimization tool
 *
 * Copyright 2020-2022 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	MIT
 *
 * Licensed under the MIT License, found in the file LICENSE and at
 * https://opensource.org/licenses/MIT
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */
#pragma once

#include <Eigen/Dense>
#include <IpTypes.hpp>
#include <tuple>
#include <vector>

namespace Optimization {
  using RowMat
      = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

  std::tuple<Eigen::Index, Eigen::Index, Eigen::VectorX<Eigen::Index>>
  make_data(
      Eigen::Index number_of_constraints_per_step,
      Eigen::Index number_of_controls_per_step,
      Eigen::Ref<Eigen::VectorXd const> const &constraint_interpolationpoints,
      Eigen::Ref<Eigen::VectorXd const> const &control_interpolationpoints);

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
    Eigen::Ref<Eigen::Vector<Eigen::Index, Eigen::Dynamic> const>
    get_block_column_offsets() const;
    Eigen::Ref<Eigen::VectorXd const> get_allvalues() const;

    Eigen::Index get_inner_col_height(Eigen::Index column) const;
    Eigen::Index get_outer_col_height(Eigen::Index column) const;
    Eigen::Index get_outer_width() const;
    Eigen::Index get_inner_width() const;
    Eigen::Index get_outer_height() const;
    Eigen::Index get_inner_height() const;

    Eigen::Index get_outer_rowstart(Eigen::Index column) const;
    Eigen::Index get_inner_rowstart(Eigen::Index column) const;

    Eigen::Index get_outer_colstart(Eigen::Index column) const;
    Eigen::Index get_inner_colstart(Eigen::Index column) const;

    Eigen::Index column_values_start(Eigen::Index column) const;
    Eigen::Index column_values_end(Eigen::Index column) const;

    void setZero();
    void setOnes();

    Eigen::Ref<RowMat> get_column_block(Eigen::Index column);
    Eigen::Ref<RowMat const> get_column_block(Eigen::Index column) const;
    Eigen::Index nonZeros() const;

    void supply_indices(
        Eigen::Ref<Eigen::VectorX<Ipopt::Index>> Rowindices,
        Eigen::Ref<Eigen::VectorX<Ipopt::Index>> Colindices) const;
    Eigen::MatrixXd whole_matrix() const;

  private:
    Eigen::Ref<Eigen::VectorXd> allvalues();

    virtual double *get_value_pointer() = 0;
    virtual double const *get_value_pointer() const = 0;

    Eigen::Index const block_height;
    Eigen::Index const block_width;
    Eigen::Vector<Eigen::Index, Eigen::Dynamic> const block_column_offsets;

  protected:
    ConstraintJacobian_Base(ConstraintJacobian_Base const &) = default;
    void assignment_helper(ConstraintJacobian_Base const &other);
  };

  bool
  operator==(ConstraintJacobian_Base const &, ConstraintJacobian_Base const &);

  bool
  operator!=(ConstraintJacobian_Base const &, ConstraintJacobian_Base const &);

  class MappedConstraintJacobian : public ConstraintJacobian_Base {
  public:
    MappedConstraintJacobian(
        double *values, Eigen::Index number_of_entries,
        Eigen::Index number_of_constraints_per_step,
        Eigen::Index number_of_controls_per_step,
        Eigen::Ref<Eigen::VectorXd const> const &constraint_interpolationpoints,
        Eigen::Ref<Eigen::VectorXd const> const &control_interpolationpoints);

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
        Eigen::Index number_of_constraints_per_step,
        Eigen::Index number_of_controls_per_step,
        Eigen::Ref<Eigen::VectorXd const> const &constraint_interpolationpoints,
        Eigen::Ref<Eigen::VectorXd const> const &control_interpolationpoints);
    ConstraintJacobian(ConstraintJacobian const &) = default;
    ConstraintJacobian(ConstraintJacobian_Base const &);

    ConstraintJacobian &operator=(MappedConstraintJacobian const &);
    ConstraintJacobian &operator=(ConstraintJacobian const &);

  private:
    Eigen::VectorXd storage;

    double *get_value_pointer() final;
    double const *get_value_pointer() const final;
  };

} // namespace Optimization
