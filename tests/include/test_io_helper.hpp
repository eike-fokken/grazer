#pragma once
#include <filesystem>

struct Catch_cout {
  Catch_cout(std::streambuf *new_buffer);

  ~Catch_cout();

private:
  std::streambuf *old;
};

class Directory_creator {
public:
  Directory_creator(std::string testdirname = "Grazer_testdir");

  ~Directory_creator();

  std::filesystem::path const &get_path() const;

private:
  std::filesystem::path const main_test_dir_path;
  static std::filesystem::path create_testdir(std::string directory_name);
};

class Path_changer {
public:
  Path_changer(std::filesystem::path testdir);
  ~Path_changer();

private:
  std::filesystem::path change_to_testdir(std::filesystem::path testdir);

  std::filesystem::path const old_current_directory;
};
