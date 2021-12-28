#pragma once
#include "Timedata.hpp"
#include <Eigen/Sparse>
#include <nlohmann/json.hpp>
#include <vector>

namespace Aux {
  class InterpolatingVector;
  class MappedInterpolatingVector;

  struct Interpolation_data {
    double first_point;
    double delta;
    size_t number_of_entries;
  };
  Interpolation_data make_from_start_delta_number(
      double first_point, double delta, int number_of_entries);

  Interpolation_data make_from_start_delta_end(
      double first_point, double desired_delta, double last_point);

  Interpolation_data make_from_start_number_end(
      double first_point, double last_point, int number_of_entries);

  class InterpolatingVector_Base {
  public:
    virtual ~InterpolatingVector_Base();
    InterpolatingVector_Base();
    InterpolatingVector_Base(
        Interpolation_data data, Eigen::Index _inner_length);
    InterpolatingVector_Base(
        std::vector<double> interpolation_points, Eigen::Index inner_length);
    InterpolatingVector_Base(
        Eigen::Ref<Eigen::VectorXd const> const &interpolation_points,
        Eigen::Index inner_length);

    InterpolatingVector_Base &operator=(InterpolatingVector_Base const &other);

  protected:
    /// @brief helper function for implementing the assignment in derived
    /// classes
    void assignment_helper(InterpolatingVector_Base const &other);
    void move_helper(InterpolatingVector_Base &&other);

    /// @brief copy constructor, only callable from derived classes for copying
    /// private data.
    InterpolatingVector_Base(InterpolatingVector_Base const &other) = default;
    // move constructor, also only callable from derived classes:
    InterpolatingVector_Base(InterpolatingVector_Base &&other);

  public:
    void set_values_in_bulk(Eigen::Ref<Eigen::VectorXd const> const &values);
    void setZero();

    void interpolate_from(InterpolatingVector_Base const &);

    Eigen::Index get_total_number_of_values() const;
    Eigen::Map<Eigen::VectorXd const> get_interpolation_points() const;

    Eigen::Index get_inner_length() const;

    Eigen::VectorXd operator()(double time) const;

    Eigen::Ref<Eigen::VectorXd const> const get_allvalues() const;

    Eigen::Ref<Eigen::VectorXd> mut_timestep(Eigen::Index index);

    void
    push_to_index(Eigen::Index index, double timepoint, Eigen::VectorXd vector);

    Eigen::Ref<Eigen::VectorXd const> vector_at_index(Eigen::Index index) const;
    double interpolation_point_at_index(Eigen::Index index) const;

    /** \brief Returns the number of timepoints or equivalently the number of
     * inner vectors.
     */
    Eigen::Index size() const;

  private:
    virtual Eigen::Ref<Eigen::VectorXd> allvalues() = 0;
    virtual Eigen::Ref<Eigen::VectorXd const> const allvalues() const = 0;

    std::vector<double> interpolation_points;
    Eigen::Index inner_length;
  };

  bool operator==(
      InterpolatingVector_Base const &lhs, InterpolatingVector_Base const &rhs);
  bool operator!=(
      InterpolatingVector_Base const &lhs, InterpolatingVector_Base const &rhs);

  /** @brief returns true if and only if both arguments have the same inner
   * length and the same interpolation points.
   */
  bool have_same_structure(
      InterpolatingVector_Base const &vec1,
      InterpolatingVector_Base const &vec2);

  class InterpolatingVector : public InterpolatingVector_Base {
  public:
    static InterpolatingVector construct_and_interpolate_from(
        Eigen::VectorXd _interpolation_points, Eigen::Index inner_length,
        InterpolatingVector_Base const &values);

    static InterpolatingVector construct_from_json(
        nlohmann::json const &json, nlohmann::json const &schema);

    InterpolatingVector();
    InterpolatingVector(Interpolation_data data, Eigen::Index inner_length);
    InterpolatingVector(
        std::vector<double> _interpolation_points, Eigen::Index inner_length);
    InterpolatingVector(
        Eigen::Ref<Eigen::VectorXd const> const &_interpolation_points,
        Eigen::Index inner_length);

    // assignment:
    InterpolatingVector &operator=(InterpolatingVector_Base const &other);
    InterpolatingVector &operator=(InterpolatingVector const &other);
    InterpolatingVector &operator=(InterpolatingVector &&other);
    // copy constructor:
    InterpolatingVector(InterpolatingVector_Base const &other);
    InterpolatingVector(InterpolatingVector const &other) = default;
    // move constructor:
    InterpolatingVector(InterpolatingVector &&other) = default;

  private:
    Eigen::Ref<Eigen::VectorXd> allvalues() final;
    Eigen::Ref<Eigen::VectorXd const> const allvalues() const final;

    Eigen::VectorXd values;
  };

  class MappedInterpolatingVector : public InterpolatingVector_Base {
  public:
    MappedInterpolatingVector(
        Interpolation_data data, Eigen::Index _inner_length, double *array,
        Eigen::Index number_of_elements);
    MappedInterpolatingVector(
        std::vector<double> interpolation_points, Eigen::Index inner_length,
        double *array, Eigen::Index number_of_elements);
    MappedInterpolatingVector(
        Eigen::Ref<Eigen::VectorXd const> const &interpolation_points,
        Eigen::Index inner_length, double *array,
        Eigen::Index number_of_elements);

    // We cannot have a copy constructor because a MappedInterpolatingVector
    // needs an underlying storage.
    MappedInterpolatingVector(MappedInterpolatingVector const &other) = delete;

    /** @brief Copy assignment operator.
     *
     * Throws if the underlying storages of
     * the left-hand operand does not have the same total size as the right-hand
     * operand.
     */
    MappedInterpolatingVector &operator=(InterpolatingVector_Base const &other);
    /** @brief Copy assignment operator.
     *
     * Throws if the underlying storages of
     * the left-hand operand does not have the same total size as the right-hand
     * operand.
     */
    MappedInterpolatingVector &
    operator=(MappedInterpolatingVector const &other);

  private:
    Eigen::Ref<Eigen::VectorXd> allvalues() final;
    Eigen::Ref<Eigen::VectorXd const> const allvalues() const final;

    Eigen::Map<Eigen::VectorXd> mapped_values;
  };

  class ConstMappedInterpolatingVector : public InterpolatingVector_Base {
  public:
    ConstMappedInterpolatingVector(
        Interpolation_data data, Eigen::Index _inner_length,
        double const *array, Eigen::Index number_of_elements);
    ConstMappedInterpolatingVector(
        std::vector<double> interpolation_points, Eigen::Index inner_length,
        double const *array, Eigen::Index number_of_elements);
    ConstMappedInterpolatingVector(
        Eigen::Ref<Eigen::VectorXd const> const &interpolation_points,
        Eigen::Index inner_length, double const *array,
        Eigen::Index number_of_elements);

    void reset_values(double const *array, Eigen::Index number_of_elements);

  private:
    Eigen::Ref<Eigen::VectorXd> allvalues() final;
    Eigen::Ref<Eigen::VectorXd const> const allvalues() const final;

    Eigen::Map<Eigen::VectorXd const> mapped_values;
  };

} // namespace Aux
