#include "test_io_helper.hpp"
#include <iostream>
#include <sstream>
#include <streambuf>

Catch_cout::Catch_cout(std::streambuf *new_buffer) :
    old(std::cout.rdbuf(new_buffer)) {}

Catch_cout::~Catch_cout() { std::cout.rdbuf(old); }

Directory_creator::Directory_creator(std::string testdirname) :
    main_test_dir_path(create_testdir(testdirname)) {}

Directory_creator::~Directory_creator() {
  try {
    std::filesystem::remove_all(main_test_dir_path);
  } catch (...) {
    std::cout << "Couldn't remove " << main_test_dir_path.string() << ".\n\n"
              << "###############################################\n"
              << "Please remove it yourself!\n"
              << "###############################################\n\n"
              << std::flush;
  }
}

std::filesystem::path const &Directory_creator::get_path() const {
  return main_test_dir_path;
}

std::filesystem::path
Directory_creator::create_testdir(std::string directory_name) {
  auto full_path = std::filesystem::temp_directory_path()
                   / std::filesystem::path(directory_name);
  auto new_dir = std::filesystem::create_directory(full_path);
  if (not new_dir) {
    std::ostringstream o;
    o << "There is already a directory named " << full_path.string()
      << ", cannot execute the test. Please remove the directory at\n"
      << full_path.string() << std::endl;
    throw std::runtime_error(o.str());
  }
  return full_path;
}

Path_changer::Path_changer(std::filesystem::path testdir) :
    old_current_directory(change_to_testdir(testdir)) {}

Path_changer::~Path_changer() {
  try {
    std::filesystem::current_path(old_current_directory);
  } catch (...) {
    std::cout << "Couldn't change back to the old directory!\n"
              << "Swallowing the exception\n"
              << "to make sure the test directory is removed, if possible."
              << std::endl;
  }
}

std::filesystem::path
Path_changer::change_to_testdir(std::filesystem::path testdir) {
  auto curr_dir = std::filesystem::current_path();
  std::filesystem::current_path(testdir);
  return curr_dir;
}
