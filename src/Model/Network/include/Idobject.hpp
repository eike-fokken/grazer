/*
 * Grazer - network simulation and optimization tool
 *
 * Copyright 2020-2022 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	MIT
 *
 * Licensed under the MIT License, found in the file LICENSE and at
 * https://opensource.org/licenses/MIT
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */
#pragma once
#include <memory>
#include <nlohmann/json.hpp>

namespace Network {
  /// \brief Provides a unified "name" to inheriting classes.
  class Idobject {
  public:
    static std::string get_type() = delete;

    static nlohmann::json get_schema();

    Idobject(std::string const &_id);

    // This class is non-copyable to prevent pointers to it from becoming
    // dangling (by using the copy constructor for an implicit move
    // constructor.)
    Idobject(Idobject const &) = delete;
    Idobject(Idobject &) = delete;
    Idobject &operator=(Idobject const &) = delete;

    // This class is immovable to prevent pointers to it from becoming
    // dangling.
    Idobject(Idobject const &&) = delete;
    Idobject(Idobject &&) = delete;
    Idobject &operator=(Idobject const &&) = delete;

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
