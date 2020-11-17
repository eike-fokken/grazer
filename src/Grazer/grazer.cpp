#include <Exception.hpp>
#include <Jsonreader.hpp>
#include <Problem.hpp>
#include <iostream>
#include <memory>

int main(int argc, char **argv) {

  if (argc != 4) {
    gthrow({" Wrong number of arguments."})
  }

  auto p = Jsonreader::setup_problem(argv[1], argv[2], argv[3]);

  p->display();
}
