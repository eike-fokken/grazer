[![Release](https://github.com/eike-fokken/grazer/actions/workflows/release.yml/badge.svg)](https://github.com/eike-fokken/grazer/releases)

## Grazer

Grazer is a software for the simulation and optimal control of dynamical systems
defined on networks. Input data is handed to grazer by json files.

Grazer is up to now used and tested by one person, so expect many rough edges.
Still, feel free to dig into it. As long as almost nobody uses it, you can
probably expect very detailed responses to questions. Any feedback in github
issues or to the mail address below is welcome.

## Installation
Before you can use grazer, you must build it. On Unix-like operating systems
(especially MacOs and Linux), building should be rather simple.

Supported compilers are clang-9 and gcc-9 as well as later versions.

In case you are interested in compiling with MSVC, support would be reinstated,
as soon as the branch msvc_ipopt works. Without prompting, don't expect
development there.

### Installing all dependencies

#### MacOS
NOTE: There are no automatic checks anymore for macos. I assume that everything
should run smoothly but I disabled the build tests on mac as I couldn't get them
to work after the github runners switched to arm mac. The following hints were
found when someone with a Mac was still contributing to grazer:

On MacOS, the most convenient way to install all of Grazer's dependencies is by
using [Homebrew](https://brew.sh/). After Homebrew is installed, entering

``` 
brew install bash git gfortran metis cmake make wget tar patch dos2unix pkg-config g++ gfortran

```

inside a terminal window will install all required dependencies.
Note that you can also use clang++ instead of g++. On MacOS it may 
also be necessary to explicitly set the system SDK by executing

``` bash
echo "export SDKROOT=$(xcrun --sdk macosx --show-sdk-path)" >> ~/.bash_profile && source ~/.bash_profile
```

inside a terminal window.

#### Linux-based OSes

On Debian-based Linux distributions (e.g. Ubuntu), the dependencies can be
installed via

```
sudo apt-get install bash git cmake make wget tar patch dos2unix pkg-config g++ gfortran libmetis-dev gfortran doxygen graphviz
```

## Building

To build grazer, clone this git repository, change into it and run

```
git submodule update --init --recursive --depth=1
```

to fetch dependencies (`--depth=1` is not needed but cuts down the size
tremendously).

Then run

```
cmake -DCMAKE_BUILD_TYPE=Release -S . -B release
```

and afterwards

```
cmake --build release
```

You can also build a debug build, just exchange 'release' with 'debug' and
'Release' with 'Debug' everywhere in the last two commands.

After the build has completed, a binary named grazer will appear in
`release/src/Grazer/`.

### Running tests
There is a test suite, which can be executed via

```
cd release && ctest ; cd ..
```

Hopefully these tests should return `100% tests passed`.

## Usage

Grazer provides a command line interface (cli).

The executable is found at `release/src/Grazer/`. Running it with `--help` 
will look something like this:

```bash
$ grazer --help
Simulation of dynamical systems
Usage: grazer [OPTIONS] SUBCOMMAND

Options:
  -h,--help                   Print this help message and exit

Subcommands:
  run                         Simulate the evolution of the given dynamical system
  schema                      Schema Helpers for the JSON Schemas validating the input files

```

As a first try after compilation, run

```
release/src/Grazer/grazer run data/base
```

from the repository directory to run a simulation of a gas-power network.

A second trial run can be

```
release/src/Grazer/grazer run data/steady_base_optimization
```

which executes an optimal control problem computing optimal compressor and valve usage for the same gas network.

You can explore all grazer commands with `--help`. Commands usually expect a
`grazer_directory` argument, which is a directory with the following structure

```txt
grazer_directory
|- problem
|  |- problem_data.json
|  |- topology.json
|  |- ...
|- schemas
|- output
```

The `schema` and `output` folder can also be generated by grazer from `grazer
schema make-x` commands and `grazer run` respectively. Not even the
`problem` folder necessarily has to be there: If there are no files describing
the `problem` yet but one has run the schema generation, `grazer schema
insert_link` will create empty problem files with just the `$schema` key
referencing the respective JSON schema. Schema-aware editors (like
[VSCode](https://code.visualstudio.com/)) will then provide you with
autocompletion when writing the simulation problem specification files.

(If you already have simulation problem specification files the same command
will only insert the `$schema` keys.)

Right now only Power and Gas networks are supported out of the box.
A few example problems can be found in the `data` subdirectory.

## Documentation

Documentation is still a work in progress

- [User Guide](https://github.com/eike-fokken/grazer/wiki/use-grazer)
- [Technical Documentation](https://eike-fokken.github.io/grazer/)

<details> <summary> Local Documentation </summary>

You can generate some documentation with
```bash
cmake --build release --target docs
```
If you have doxygen installed, this will generate technical documentation under
```
release/docs/html/index.html.
```
although it is not yet very complete.
If you have pdflatex and the packages listed in `docs/userguide.tex` installed, a file
```
release/docs/userguide.pdf
```
will appear. Note that the userguide is also not yet very usable.

</details>


## Feedback
Feedback is welcome! Just open a github issue or write an email to

e.fokken+grazer@posteo.de





