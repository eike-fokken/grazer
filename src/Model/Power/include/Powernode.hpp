#pragma once
#include <Boundaryvalue.hpp>
#include <Equationcomponent.hpp>
#include <string>

namespace Model::Networkproblem::Power {

  class Powernode : public Equationnode {

  public:
    Powernode(std::string id,
              std::map<double, Eigen::Matrix<double, 2, 1>> _boundary_values,
              double _G, double _B)
        : Equationnode(id), boundaryvalue(_boundary_values), G(_G), B(_B){};

    virtual ~Powernode(){};

    virtual int get_number_of_states() const override final;

    double get_G() const;
    double get_B() const;

  protected:
    Boundaryvalue<Powernode, 2> boundaryvalue;
    /// Real part of the admittance of this node
    double G;
    /// Imaginary part of the admittance of this node
    double B;
  };

} // namespace Model::Networkproblem::Power
