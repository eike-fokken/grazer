## Grazer

Grazer is a program that shall simulate the time evolution of dynamical systems.
It does so by a 'First-discretize-then-optimize' approach.
Input data is handed to grazer by json files.

This software is in alpha-phase and not yet suited for use.
Feel free though to dig into it.
It will mature over the course of 2021.


## Installation

Before you can use grazer, you must build it.
Supported compilers are clang and gcc.
Note that also gcc-mingw can be used but you have to ship the dlls by hand.
Later on there will be a guide for this.

As we strive for full standard compliance, a port to other compilers is hopefully easy.


To do so, clone this git repository, change into it and run

```git submodule update --init eigen googletest nlohmann_json```

to fetch dependencies.

Then run

```cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -S . -B release```

and afterwards

```cmake --build release```

You can also build a debug build, just exchange 'release' with 'debug' and 'Release' with 'Debug'
everywhere in the last two commands.

After the build has completed, a binary named grazer (or grazer.exe , if built with mingw compiler)
will appear in `release/src/Grazer/`.

## Usage

Up to now only Power networks are supported.
You can pass network data, initial conditions and boundary conditions in three json files.
Example files (that also contain gas data, which is not yet read) are in `data/`.

Invoking grazer can be done like this:

```grazer topology.json initial.json boundary.json delta_t delta_x T```

Here the topology file declares the graph structure and nature of the edges and nodes.
See `data/topology_pretty.json` for an example.
`boundary.json` declares boundary conditions and `initial.json` provides initial conditions for
time `t = -delta_t`. For power-only problems one still has to provide initial values as they are also
used for an initial guess in the Newton method.

`delta_t` is the desired time step, `T` the last time point.
If `T/delta_t` is not an integer, the next smaller time step is used.
`delta_x` is the desired spatial step size. In a similar fashion to the time step the next smaller step size is used
such that the length of each element is an integer multiple of the step size.

Calling grazer without arguments is equivalent to calling:

``` grazer topology_pretty.json initial_pretty.json boundary_pretty.json 1800 10000 8640```



## Feedback
Feedback is welcome! Just open a github issue or write an email to

e.fokken+grazer@posteo.de





