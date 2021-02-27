#pragma once
#include "Componentchooser.hpp"
#include "Flowboundarynode.hpp"
#include "Innode.hpp"

namespace Model::Networkproblem::Componentfactory {

  std::unique_ptr<Network::Node> build_innode(nlohmann::json const &topology);

  struct Innodebuilder final : public Nodedatabuilder_base {
    
    std::string get_type() override { return "Innode"; };

  private:
    Nodefactory constructer_pointer() const override {
      return build_innode;
    };
  };
  }
