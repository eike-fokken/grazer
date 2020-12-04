#include <Mathfunctions.hpp>

namespace Aux {

  double epsilon=1e-4;

  double smooth_abs(double x) {

    if(x<=-epsilon){
      return -x;
    } else if( x>=epsilon){
      return x;
    } else {
      double rel =x / epsilon;
      return x*rel/2.0*(-rel*rel +3);
    }

  }

  double dsmooth_abs_dx(double x){

    if (x <= -epsilon) {
      return -1.0;
    } else if (x >= epsilon) {
      return 1.0;
    } else {
      double rel = x / epsilon;
      return rel*(-2*rel*rel+3);
    }
  }

}
