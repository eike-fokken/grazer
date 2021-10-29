#pragma once
#include <Eigen/Sparse>
#include <nlohmann/json.hpp>
// #include "Equationcomponent.hpp"

namespace Model {
  class SimpleStatecomponent;

  class Statecomponent {
    /** \brief SimpleStatecomponent is a friend of Statecomponent to give it
     * access to #state_startindex and #state_afterindex.
     */
    friend class SimpleStatecomponent;
    friend class Networkproblem;

  public:
    static nlohmann::json get_initial_schema() = delete;

    /** \brief This function sets the indices #state_startindex and
     * #state_afterindex.
     *
     * @param next_free_index The first state index that is currently not
     * claimed by another component.
     * @returns The new lowest free index.
     */
    virtual Eigen::Index set_state_indices(Eigen::Index next_free_index) = 0;

    /** @brief returns #state_afterindex - #state_startindex.
     */
    Eigen::Index get_number_of_states() const;

    /** \brief getter for #state_startindex
     */
    Eigen::Index get_state_startindex() const;

    /** \brief getter for #state_afterindex
     */
    Eigen::Index get_state_afterindex() const;

    /** \brief Stores computed values inside the component.
     */
    virtual void
    json_save(double time, Eigen::Ref<Eigen::VectorXd const> const &state)
        = 0;

    /** @brief Add the results saved inside the component in the general result
     * json.
     */
    virtual void add_results_to_json(nlohmann::json &overall_result_json) = 0;

    /** \brief Fills the indices owned by this component with initial values
     * from a json.
     *
     * @param[out] new_state state vector, which shall contain the initial
     * values afterwards.
     * @param initial_json json object that contains the initial values.
     */
    virtual void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::json const &initial_json)
        = 0;

  protected:
    /** @brief fills the #component_output json with some pre-defined
     * properties, like the component id.
     * @param id id of the component.
     */
    void setup_output_json_helper(std::string const &id);

    /** @brief getter for #component_output.
     */
    nlohmann::json &get_output_json_ref();

  private:
    /** \brief holds the computed values of this component.
     *
     * Contains exactly two keys: "time" and "data"
     * Time holds a number representing the time in seconds.
     * Data holds an array of jsons, whose layout depends on the component in
     * question.
     */
    nlohmann::json component_output;

    /** \brief The first index, this Equationcomponent "owns".
     *
     * Most equation components write only to their own indices between
     * #state_startindex (inclusive) and #state_afterindex (exclusive).
     * There are exceptions though, e.g. instances of
     * \ref Model::Gas::Gasnode "Gasnode".
     */
    Eigen::Index state_startindex{-1};

    /** \brief The first index after #state_startindex, that is not "owned" by
     * this Equationcomponent.
     *
     * Most equation components write only to their own indices between
     * #state_startindex (inclusive) and #state_afterindex (exclusive).
     * There are exceptions though, e.g. instances of
     * \ref Model::Gas::Gasnode "Gasnode".
     */
    Eigen::Index state_afterindex{-1};
  };
} // namespace Model
