#include "Indexmanager.hpp"
#include "Initialvalue.hpp"
#include "schema_validation.hpp"

namespace Aux {

  Indexmanager::~Indexmanager() {}

  int Indexmanager::get_startindex() const { return startindex; }
  int Indexmanager::get_afterindex() const { return afterindex; }
  int Indexmanager::get_number_of_indices() const {
    return afterindex - startindex;
  }
  int Indexmanager::set_indices(
      int next_free_index, int number_of_needed_indices) {
    startindex = next_free_index;
    afterindex = next_free_index + number_of_needed_indices;
    return afterindex;
  }

  template <int Values_per_step>
  void Timeless_Indexmanager<Values_per_step>::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> vector_to_be_filled,
      nlohmann::json const &initial_json,
      nlohmann::json const &initial_schema) {

    Aux::schema::validate_json(initial_json, initial_schema);
    Initialvalue<Values_per_step> initialvalues(initial_json);
    for (int i = 0; i != number_of_points; ++i) {
      try {
        new_state.segment<2>(get_start_state_index() + 2 * i)
            = bl->state(bl->p_qvol_from_p_qvol_bar(initialvalues(i * Delta_x)));
      } catch (...) {
        std::cout << "could not set initial value of pipe " << get_id() << ".\n"
                  << "Requested point was " << i * Delta_x << ". \n"
                  << "Length of line is " << get_length() << std::endl;
        std::cout << "requested - length: " << (i * Delta_x - get_length())
                  << std::endl;
        throw;
      }
    }
  }

  template <int Values_per_step>
  void Timed_Indexmanager<Values_per_step>::set_initial_values(
      Model::Timedata timedata, Aux::Controller &vector_controller_to_be_filled,
      nlohmann::json const &initial_json,
      nlohmann::json const &initial_schema) {
    Aux::schema::validate_json(initial_json, initial_schema);
  }

  template class Timeless_Indexmanager<1>;
  template class Timeless_Indexmanager<2>;
  template class Timeless_Indexmanager<4>;

} // namespace Aux
