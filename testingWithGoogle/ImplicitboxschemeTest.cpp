#include <Isothermaleulerequation.hpp>
#include <Implicitboxscheme.hpp>
#include <gtest/gtest.h>



TEST(testImplicitboxschem, evaluate_point) {


  Model::Scheme::Implicitboxscheme scheme();
  double Area = 2.0;
  double diameter = 3.5;
  double roughness = 1.0;
  Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter, roughness);


}

