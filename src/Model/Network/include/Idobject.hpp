#pragma once
#include <string>

namespace Network {

  class Idobject {
  public:
    Idobject(std::string _id) : id(_id){};

    virtual ~Idobject(){};

    virtual void display() const;

    /// This function returns the id of the object.
    std::string get_id() const;

  protected:
    void print_id() const;

  private:
    /// A unique id for the node.
    std::string id;
  };

} // namespace Network
