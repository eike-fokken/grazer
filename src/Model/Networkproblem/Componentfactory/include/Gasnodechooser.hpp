#pragma once
#include "Componentchooser.hpp"

namespace Model::Networkproblem::Componentfactory {

  std::unique_ptr<Network::Node> build_innode(nlohmann::json const &topology);
  std::unique_ptr<Network::Node> build_source(nlohmann::json const &topology);
  std::unique_ptr<Network::Node> build_sink(nlohmann::json const &topology);


  struct Innodebuilder final : public Nodedatabuilder_base {
    
    std::string get_type() override { return "Innode"; };

  private:
    Nodefactory constructer_pointer() const override {
      return build_innode;
    };
  };

  
  struct Sourcebuilder final : public Nodedatabuilder_base {

    std::string get_type() override { return "Source"; };

  private:
    Nodefactory constructer_pointer() const override { return build_source; };
  };

  
  struct Sinkbuilder final : public Nodedatabuilder_base {

    std::string get_type() override { return "Sink"; };

  private:
    Nodefactory constructer_pointer() const override { return build_sink; };
  };
} // namespace Model::Networkproblem::Componentfactory
