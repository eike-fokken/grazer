#pragma once
#include "Bernoulligasnode.hpp"
#include "Flowboundarynode.hpp"

namespace Model::Networkproblem::Gas {

  template <typename Flowboundarynode>
  class Sink final : public Flowboundarynode {

    public:
      static std::string get_type();

      Sink(nlohmann::json const &data);

    private:
      static nlohmann::json
      revert_boundary_conditions(nlohmann::json const &data);
    };


  extern template class Sink<Pressurecouplingnode>;
  extern template class Sink<Bernoullicouplingnode>;
  } // namespace Model::Networkproblem::Gas
