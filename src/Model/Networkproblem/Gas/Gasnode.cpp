#include <Gasedge.hpp>
#include <Gasnode.hpp>

namespace Model::Networkproblem::Gas {

  void Gasnode::setup() {
    writable_indices = std::vector<int>(0);
    for (auto &startedge : get_starting_edges()) {
      auto startgasedge = dynamic_cast<Gasedge *>(startedge);
      writable_indices.push_back(startgasedge->give_away_start_index());
    }
    for (auto &endedge : get_ending_edges()) {
      auto endgasedge = dynamic_cast<Gasedge *>(endedge);
      writable_indices.push_back(endgasedge->give_away_end_index());
    }
  }

  int Gasnode::get_number_of_states() const { return 0;}

} // namespace Model::Networkproblem::Gas
