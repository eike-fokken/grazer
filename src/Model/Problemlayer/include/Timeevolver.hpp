#include <Eigen/Sparse>
#include <utility>

namespace Model {

  class Timeevolver {
  public:

    Timeevolver();

    void make_step();



  private:

    /// Put in cpp and maybe refactor to free helper function

    int init_N(double _Full_time_interval, double desired_delta_t) const{
      int const _N = static_cast<int>(std::ceil(_Full_time_interval / desired_delta_t));
      return _N;
    }

    double init_delta_t(double _Full_time_interval, int _N) const {
      return _Full_time_interval/_N;
    }

    Eigen::VectorXd new_state;
    Eigen::VectorXd last_state;


    // Jsonreader::set_initial_values(state1, initial, p);

    double last_time;
    double new_time;
    double const starttime;
    double const endtime;
    double const Full_time_interval{endtime - starttime};
    int const N;
    double const delta_t;
    
  };

}
