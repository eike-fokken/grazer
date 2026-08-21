# AGENTS.md

## Project Overview

Grazer is a C++17 application for the simulation and optimal control of dynamical
systems defined on networks (currently gas and power networks). Input is provided
via JSON files, which are validated against JSON schemas. It is built with CMake
and exposes a command-line interface.

## Build & Test Commands

These are the standard build and test commands. Assume they work as-is; do not
investigate build-system internals (e.g. how Ipopt/coinbrew is fetched and built).

```bash
git submodule update --init --recursive --depth=1
cmake -DCMAKE_BUILD_TYPE=Release -S . -B release
cmake --build release
cd release && ctest
```

- The `grazer` binary appears at `release/src/Grazer/grazer`.
- For a debug build, replace `release` with `debug` and `Release` with `Debug`.
- Run tests with `ctest` from inside the build directory
  (`cd release && ctest`).
- **If `cmake --build <buildfolder>` fails, stop and report back to the user
  rather than attempting to debug or fix the build system.**

## Repository Structure

- `src/Grazer/` — the `grazer` executable (CLI) plus the `commands` library
  (`commands.cpp`, `helpers.cpp`, `optimize.cpp`).
- `src/Model/` — core domain model:
  - `Network/` — node/edge/id object primitives (`Node`, `Edge`, `Net`, `Idobject`).
  - `Networkproblem/` — problem definitions for `Gas/`, `Power/`, and
    `Gaspowerconnection/` (gas-power coupling) plus `Componentclasses/`.
  - `Scheme/` — numerical time-integration schemes.
  - `Balancelaw/` — balance-law / PDE descriptions (e.g. `Isothermaleulerequation`).
  - `Netfactory/`, `Componentfactory/` — factories that construct networks and
    components (with `Full_factory/`, `Gas_factory/`, `Power_factory/`).
  - `Problemlayer/` — the top-level problem/time-evolution layer.
- `src/Auxillary/` — supporting utilities:
  - `aux_json/`, `json_schema/`, `ComponentJsonHelpers/`, `Input_Output/` — JSON
    and I/O handling.
  - `Mathfunctions/`, `Matrixhandler/`, `InterpolatingVector/` — math/linear algebra.
  - `Stochastics/`, `Timedata/`, `unit_conversion/`, `Misc/`, `Exception/`,
    `Coloroutput/`.
- `src/IpoptWrapper/` — wrapper/adaptor around the Ipopt optimizer.
- `src/Newtonsolver/` — Newton solver for nonlinear systems.
- `src/Optimization_helpers/` — caching and helpers for optimization
  (e.g. `ControlStateCache`, `EquationDerivativeCache`).

Top-level directories:

- `data/` — example problem directories (e.g. `base/`, `base_optimization/`,
  `steady_base_optimization/`). Each problem directory has the layout
  `problem/`, `schemas/`, `output/`.
- `tests/` — gtest-based test suite (mirrors `src/` layout).
- `helper_functions/` — standalone utility executables (e.g. plotting/IO helpers).
- `json_schemas/` — JSON schemas validating input files.
- `docs/` — documentation (Doxygen + LaTeX).

## Code Conventions

- C++17, compiled with GCC or Clang (MSVC is not currently supported).
- Headers for each module live in an `include/` subdirectory next to the `.cpp`
  files.
- Follow existing namespaces, naming conventions, and library targets already
  defined in the CMake setup; do not introduce new dependencies without need.
- Do not add code comments unless asked.

## Notes / Gotchas

- Debug builds enable strict warnings (treated as errors) and `_GLIBCXX_DEBUG`.
- The project version is defined in `Version.cmake`; update `CHANGELOG.md` when
  the version changes.
- JSON input is validated against schemas in `json_schemas/`.