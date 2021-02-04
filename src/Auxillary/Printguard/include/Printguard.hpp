#include <Problem.hpp>
#include <memory>

namespace Aux {

  /// This is an RAII class that is instantiated before any data is produced and
  /// has scope to the end of all data production, so that whenever an exception
  /// escapes the program execution all data already computed is saved to the
  /// respective files.
  class Printguard {
  public:
    Printguard() = delete;
    Printguard(std::shared_ptr<Model::Problem> &_p) : p(_p){};

    ~Printguard();

  private:
    std::shared_ptr<Model::Problem> p;
  };

} // namespace Aux
