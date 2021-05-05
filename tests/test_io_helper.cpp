#include "test_io_helper.hpp"
#include <iostream>
#include <pcg_extras.hpp>
#include <pcg_random.hpp>
#include <random>
#include <randutils.hpp>
#include <sstream>
#include <streambuf>
#include <string>

static std::string create_random_name() {

  randutils::auto_seed_256 seed_source;
  pcg64 rng(seed_source);

  std::string ascii
      = "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
  int length = static_cast<int>(ascii.length() - 1);
  std::uniform_int_distribution<> dist(0, length);
  std::string randstring;
  for (unsigned int i = 0; i != 20; ++i) {

    auto a = static_cast<unsigned int>(dist(rng));
    randstring += ascii[a];
  }
  return randstring;
}

Catch_cout::Catch_cout(std::streambuf *new_buffer) :
    old(std::cout.rdbuf(new_buffer)) {}

Catch_cout::~Catch_cout() { std::cout.rdbuf(old); }

Directory_creator::Directory_creator(std::string testdirname) :
    main_test_dir_path(create_testdir(testdirname)) {}

Directory_creator::Directory_creator() :
    main_test_dir_path(
        create_testdir("Grazer_testdir_" + create_random_name())) {}

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
