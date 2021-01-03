#include <Timeevolver.hpp>

namespace Model {

  Timeevolver::Timeevolver(double _starttime, double _endtime,
                           double desired_delta_t)
      : starttime(_starttime), endtime(_endtime),
        Full_time_interval(endtime - starttime),Number_of_timesteps(init_Number_of_timesteps(desired_delta_t)),delta_t(init_delta_t())
  {}

  int Timeevolver::init_Number_of_timesteps(double desired_delta_t) const {
    int const _Number_of_timesteps =
        static_cast<int>(std::ceil(Full_time_interval / desired_delta_t));
    return _Number_of_timesteps;
  }

  double Timeevolver::init_delta_t() const {
    return Full_time_interval / Number_of_timesteps;
  }
}
