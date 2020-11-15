#include <Exception.hpp>
#include <iostream>

int main() {
  try {
    try {
      gthrow({"asdfasdfasdf"})
    } catch (const Exception &e) {
      std::cout << e.what() << std::endl;
      gthrow({e.rawmsg})
    }
  } catch (const Exception &e) {
    std::cout << e.what() << std::endl;
  }
}
