#pragma once
#include <memory>
#include <nlohmann/json.hpp>

namespace Network {
  /// \brief Provides a unified "name" to inheriting classes.
  class Idobject {
  public:
    static char const constexpr *get_type() = delete;

    static nlohmann::json get_schema();

    Idobject(std::string const &_id);

    /// \brief Getter for a const reference to the id.
    std::string const &get_id() const;

    /// \brief Getter for a copy of the id.
    std::string get_id_copy() const;

  private:
    /// \brief pointer to the actual string holding the id.
    ///
    /// This is a unique pointer instead of the string itself to save space on
    /// the stack. The price for dereferencing is acceptable because the id is
    /// used only during construction of a network and during writing out the
    /// data to the output files.
    std::unique_ptr<std::string const> const idptr;
  };

} // namespace Network
