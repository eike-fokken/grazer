[![Release](https://github.com/eike-fokken/grazer/actions/workflows/release.yml/badge.svg)](https://github.com/eike-fokken/grazer/releases)

## Grazer

Grazer is a program that shall simulate the time evolution of dynamical systems.
Input data is handed to grazer by json files.

This software is up to now only tested by one person, so expect some rough edges.
Yet feel free to dig into it.  Any feedback in github issues or to the mail address below is welcome.



## Installation
Before you can use grazer, you must build it (You can also download a prebuilt version, although note that the latex documentation is not included and also the helper functions are missing. But for a first glimpse, checkout [Releases](https://github.com/eike-fokken/grazer/releases)).
Supported compilers are clang-9, gcc-9 and Microsofts Visual Studio 2019 as well as later versions.
Other compilers may work and probably do as we strive for full standard compliance to C++17.

### Installing all dependencies

#### macOS

On macOS, the most convenient way to install all of Grazer's dependencies is by
using [Homebrew](https://brew.sh/). After Homebrew is installed, entering

``` 
brew install bash git gfortran metis cmake
```

inside a terminal window will install a required dependencies. On macOS it may 
also be necessary to explicitly set the system SDK by executing

``` bash
echo "export SDKROOT=$(xcrun --sdk macosx --show-sdk-path)" >> ~/.bash_profile && source ~/.bash_profile
```

inside a terminal window.

#### Linux-based OSes

On linux distributions like Ubuntu, we only need the metis library and a Fortran compiler.
Both can be installed by

```
sudo apt-get install libmetis-dev gfortran
```


## Installing grazer

To build grazer, clone this git repository, change into it and run

```
git submodule update --init --recursive --depth=1
```

to fetch dependencies (`--depth=1` is not needed but cuts down the size tremendously).

Then run

```
cmake -DCMAKE_BUILD_TYPE=Release -S . -B release
```

and afterwards

```
cmake --build release
```

You can also build a debug build, just exchange 'release' with 'debug' and 'Release' with 'Debug'
everywhere in the last two commands.

After the build has completed, a binary named grazer (or grazer.exe , if built on Windows)
will appear in `release/src/Grazer/`.

## Usage

Grazer provides both a command line interface (cli) and graphical user interface
(gui) both binaries can be downloaded from [our GitHub releases
page](https://github.com/eike-fokken/grazer/releases).

### Command Line Interface (CLI)

Grazer has a command line interface - running the executable with `--help` 
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

```bash
release/src/Grazer/grazer run data/base
```
from the reposiroty directory (or `.../grazer.exe ...` under Windows) to run a simulation of a gas-power network.

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


### Graphical User Interface (GUI)

Grazer uses [Gooey](https://github.com/chriskiehl/Gooey) to provide a thin
GUI wrapper for the command line interface. 

![Grazer-Gui](./docs/images/grazer-gui.png)

> Note: Since Gooey is a python package
it would require an installation of python. To avoid this dependency we use
[pyinstaller](https://www.pyinstaller.org/) to package everything into an
executable. But this does mean we essentially ship a full python version with
the GUI, which increases Grazers size from something like 2mb to something like
20mb on windows and mac and even larger on linux.

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





