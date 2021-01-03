#include <Eigen/Sparse>
#include <utility>

namespace Model {

  class Timeevolver {
  public:

    Timeevolver(double starttime, double endtime, double desired_delta_t);


    void simulate();

    void make_step();



  private:

    /// Put in cpp and maybe refactor to free helper function

    int init_Number_of_timesteps (double desired_delta_t) const ;

    double init_delta_t() const ;

    Eigen::VectorXd new_state;
    Eigen::VectorXd last_state;


    // Jsonreader::set_initial_values(state1, initial, p);

    double last_time;
    double new_time;
    double const starttime;
    double const endtime;
    double const Full_time_interval{endtime - starttime};
    int const Number_of_timesteps;
    double const delta_t;
    
  };

}
