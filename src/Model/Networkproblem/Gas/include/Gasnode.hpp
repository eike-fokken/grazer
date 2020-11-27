#pragma once
#include <Node.hpp>
#include <Equationcomponent.hpp>



namespace Model::Networkproblem::Gas {

  class Gasnode: public Network::Node, public Equationcomponent {

  public:

    /// Claims outer indices of attached edges.
    void setup() override;

    /// Returns zero, because the indices are owned by the attached edges, see
    /// setup().
    int get_number_of_states() const override;

  private:
    std::vector<int> writable_indices;

  };


}
