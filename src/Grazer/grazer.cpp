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
#include "commands.hpp"
#include "schema_generation.hpp"
#include "schema_key_insertion.hpp"

#include <CLI/CLI.hpp>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string>

// Preprocessor magic...
#define STRING(s) #s
#define GETSTRING(s) STRING(s)

int main(int argc, char **argv) {
  try {
    CLI::App app{"Simulation of dynamical systems", "grazer"};

    app.require_subcommand();

    std::string versionstring
        = "Grazer version\n" + std::string(GETSTRING(GRAZER_VERSION));
    app.set_version_flag("-v,--version", versionstring);

    CLI::App *grazer_run = app.add_subcommand(
        "run", "Simulate the evolution of the given dynamical system");

    std::filesystem::path grazer_dir;
    grazer_run
        ->add_option<std::filesystem::path, std::string>(
            "grazer-directory", grazer_dir,
            "directory with problem specification")
        ->required();

    grazer_run->callback([&]() { return grazer::run(grazer_dir); });

    CLI::App *grazer_schema = app.add_subcommand(
        "schema",
        "Schema Helpers for the JSON Schemas validating the input files");
    grazer_schema->require_subcommand();

    CLI::App *g_schema_make_full_factory = grazer_schema->add_subcommand(
        "make-full-factory", "Create the Schemas for the Full Factory problem");
    g_schema_make_full_factory
        ->add_option<std::filesystem::path>(
            "grazer-directory", grazer_dir,
            "directory with problem specification")
        ->required();
    g_schema_make_full_factory->callback(
        [&]() { return Aux::schema::make_full_factory_schemas(grazer_dir); });

    CLI::App *g_schema_insert_key = grazer_schema->add_subcommand(
        "insert-key",
        "Insert the \"$schema\" key with the appropriate schema into the "
        "problem "
        "JSONs");
    g_schema_insert_key
        ->add_option<std::filesystem::path, std::string>(
            "grazer-directory", grazer_dir,
            "directory with problem specification")
        ->required();
    g_schema_insert_key->callback([&]() {
      return Aux::schema::insert_schema_key_into_data_jsons(grazer_dir);
    });

    try {
      app.parse(argc, argv);
      return 0;
    } catch (const CLI::ParseError &e) { return app.exit(e); }
  } catch (std::exception const &ex) {
    std::cout << "\n[Error]: " << ex.what() << std::endl;
  } catch (...) {
    std::cout << "An unkown type of exception was thrown.\n\n"
                 "This is a bug, please report it!\n\n"
              << std::endl;
  }
  return 1;
}
