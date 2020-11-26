#include <Edge.hpp>
#include <Eigen/Sparse>
#include <Equationcomponent.hpp>

namespace Model::Networkproblem::Gas {

  /// @brief This class template is a base class for all Edges, that carry
  /// spatial information.
  ///
  /// Be careful: It is not really suited for odd Nofstates or rather for any
  /// situation where not exactly half of the number of states is gifted to to
  /// starting and ending nodes.
  ///
  /// @tparam Nofstates Number of states per space point, e.g. 2 (pressure and
  /// flow) for isothermal Euler equations.
  template <int Nofstates>
  class Gasedge : public Network::Edge, public Equationcomponent {
    // This constrains the template parameter to even values.
    static_assert(Nofstates%2 == 0);
  public:
    using Edge::Edge;

    int get_equation_start_index() const {
      return get_starting_state_index() + Nofstates/2;
    };

    int get_equation_after_index() const {
      return get_after_state_index() - Nofstates / 2 + 1 ;
    };

    int get_starting_state_index() const { return get_start_state_index(); };

    int get_ending_state_index() const {
      return get_after_state_index() - Nofstates;
    };

    Eigen::Matrix<double, Nofstates, 1>
    get_starting_state(Eigen::VectorXd const &state) const {
      Eigen::Matrix<double, Nofstates, 1> starting_state =
          state.segment<Nofstates>(get_starting_state_index());
      return starting_state;
    }

    Eigen::Matrix<double, Nofstates, 1>
    get_ending_state(Eigen::VectorXd const &state) const {
      Eigen::Matrix<double, Nofstates, 1> ending_state =
          state.segment<Nofstates>(get_ending_state_index() - Nofstates);
      return ending_state;
    };
  };

} // namespace Model::Networkproblem::Gas
