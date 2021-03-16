#pragma once
#include <string>
#include <vector>

[[noreturn]] void exception_builder(std::vector<std::string> stringvector,
                                    char const *file, int line);

#define gthrow(...) exception_builder(__VA_ARGS__, __FILE__, __LINE__);
