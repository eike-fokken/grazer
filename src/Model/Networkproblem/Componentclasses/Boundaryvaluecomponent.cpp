#include "Boundaryvaluecomponent.hpp"
#include "Exception.hpp"

namespace Model {

  nlohmann::json Boundaryvaluecomponent::get_boundary_schema() {
    gthrow(
        {"The fact, that you see this error message means,that a "
         "Boundarycomponent did not provide a static get_boundary_schema() "
         "method."});
  }
} // namespace Model
