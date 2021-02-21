#pragma once
#include <memory>
#include <string>

namespace Network {

  class Idobject {
  public:
    Idobject(std::string const &_id);


    /// This function returns the id of the object.
    std::string get_id() const;

  protected:

    ~Idobject(){};

  private:
    /// A unique id for the node.
    std::unique_ptr<std::string> idptr;
  };

} // namespace Network
