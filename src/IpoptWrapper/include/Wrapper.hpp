#pragma once

#include "ControlStateCache.hpp"

#include <Eigen/Sparse>
#include <limits>
#include <nlohmann/json.hpp>

#include <IpTNLP.hpp>

namespace Aux {
  class InterpolatingVector;
}

namespace Model {
  class Timeevolver;
  class OptimizableObject;
} // namespace Model

namespace Optimization {

  class IpoptWrapper final : public Ipopt::TNLP {

  public:
    IpoptWrapper(
        Model::Timeevolver &evolver, Model::OptimizableObject &problem,
        std::vector<double> simulation_timepoints,
        Eigen::Ref<Eigen::VectorXd const> const &initial_state,
        Aux::InterpolatingVector const &initial_controls,
        Aux::InterpolatingVector const &lower_bounds,
        Aux::InterpolatingVector const &upper_bounds,
        Aux::InterpolatingVector const &constraints_lower_bounds,
        Aux::InterpolatingVector const &constraints_upper_bounds);

    ~IpoptWrapper() = default;

    Eigen::VectorXd const &get_solution() const { return solution; }
    double get_final_objective_value() const { return final_objective_value; }

    // Internal Ipopt methods

    /** @name Overloaded from TNLP */

    /** Method to return some info about the nlp */
    bool get_nlp_info(
        Ipopt::Index &n, Ipopt::Index &m, Ipopt::Index &nnz_jac_g,
        Ipopt::Index &nnz_h_lag, IndexStyleEnum &index_style) final;

    /** Method to return the bounds for my problem */
    bool get_bounds_info(
        Ipopt::Index /*n*/, Ipopt::Number *x_l, Ipopt::Number *x_u,
        Ipopt::Index /*m*/, Ipopt::Number *g_l, Ipopt::Number *g_u) final;

    /** Method to return the starting point for the algorithm */
    bool get_starting_point(
        Ipopt::Index /* n */, bool /* init_x */, Ipopt::Number *x,
        bool /* init_z */, Ipopt::Number * /* z_L */, Ipopt::Number * /* z_U */,
        Ipopt::Index /*m */, bool /* init_lambda */,
        Ipopt::Number * /* lambda */) final;

    /** Method to return the objective value */
    bool eval_f(
        Ipopt::Index n, const Ipopt::Number *x, bool /* new_x */,
        Ipopt::Number &obj_value) final;

    /** Method to return the gradient of the objective */
    bool eval_grad_f(
        Ipopt::Index n, const Ipopt::Number *x, bool /* new_x */,
        Ipopt::Number *grad_f) final;

    /** Method to return the constraint residuals */
    bool eval_g(
        Ipopt::Index n, const Ipopt::Number *x, bool /* new_x */,
        Ipopt::Index m, Ipopt::Number *g) final;

    /** Method to return:
     *   1) The structure of the Jacobian (if "values" is NULL)
     *   2) The values of the Jacobian (if "values" is not NULL)
     */
    bool eval_jac_g(
        Ipopt::Index n, const Ipopt::Number *x, bool /* new_x */,
        Ipopt::Index /* m */, Ipopt::Index /* nele_jac */, Ipopt::Index *iRow,
        Ipopt::Index *jCol, Ipopt::Number *values) final;

    /** Method to return:
     *   1) The structure of the Hessian of the Lagrangian (if "values" is NULL)
     *   2) The values of the Hessian of the Lagrangian (if "values" is not
     * NULL)
     */
    // virtual bool eval_h(Index n, const Number* x, bool new_x, Number
    // obj_factor,
    //                     Index m, const Number* lambda, bool new_lambda,
    //                     Index nele_hess, Index* iRow, Index* jCol,
    //                     Number* values);

    /** This method is called when the algorithm is complete so the TNLP can
     * store/write the solution */
    void finalize_solution(
        Ipopt::SolverReturn /* status */, Ipopt::Index n,
        const Ipopt::Number *x, const Ipopt::Number * /* z_L */,
        const Ipopt::Number * /* z_U */, Ipopt::Index /* m */,
        const Ipopt::Number * /* g */, const Ipopt::Number * /* lambda */,
        Ipopt::Number obj_value, const Ipopt::IpoptData * /* ip_data */,
        Ipopt::IpoptCalculatedQuantities * /* ip_cq */) final;

  private:
    Model::OptimizableObject &problem;
    ControlStateCache cache;

    std::vector<double> const simulation_timepoints;

    Eigen::VectorXd const initial_state;

    Aux::InterpolatingVector const initial_controls;
    Aux::InterpolatingVector const lower_bounds;
    Aux::InterpolatingVector const upper_bounds;
    Aux::InterpolatingVector const constraints_lower_bounds;
    Aux::InterpolatingVector const constraints_upper_bounds;

    Eigen::VectorXd solution;
    double final_objective_value{std::numeric_limits<double>::max()};

    /** \brief returns the number of non-zeros in the constraint
     * jacobian.
     */
    Ipopt::Index nnz_constraint_jacobian();
    /** \brief fills in the row and column index vectors at the beginning of the
     * optimization.
     */
    void constraint_jacobian_structure(
        Ipopt::Index number_of_nonzeros_in_constraint_jacobian,
        Ipopt::Index *Rows, Ipopt::Index *Cols);

    /** \brief fills in the value vector for the constraint jacobian.
     */
    void constraint_jacobian(Ipopt::Number *values);

    Eigen::Index get_number_of_controls() const;
    Eigen::Index get_number_of_control_timesteps() const;
    Eigen::Index get_number_of_constraint_timesteps() const;
  };
} // namespace Optimization
