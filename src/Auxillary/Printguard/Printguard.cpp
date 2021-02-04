#include <Printguard.hpp>

namespace Aux {

  Printguard::~Printguard() { p->print_to_files(); }
} // namespace Aux
