## Grazer

Grazer is a program that shall simulate the time evolution of dynamical systems.
Input data is handed to grazer by json files.

This software is up to now only tested by one person, so expect some rough edges.
Yet feel free to dig into it.  Any feedback in github issues or to the mail address below is welcome.



## Installation
Before you can use grazer, you must build it.
Supported compilers are clang-9, gcc-9 and Microsofts Visual Studio 2019 as well as later version.
Other compilers may work and probably do as we strive for full standard compliance to C++17.

To build grazer, clone this git repository, change into it and run

```git submodule update --init --recursive --depth=1```

to fetch dependencies (`--depth=1` is not needed but cuts done the size tremendously.)

Then run

```cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -S . -B release```

and afterwards

```cmake --build release```

You can also build a debug build, just exchange 'release' with 'debug' and 'Release' with 'Debug'
everywhere in the last two commands.

After the build has completed, a binary named grazer (or grazer.exe , if built with mingw compiler)
will appear in `release/src/Grazer/`.

## Usage

Up to now only Power and Gas networks are supported.
You can pass a simulation problem in form of a json file.
A few of these files can be found in the `data` subdirectory.

Invoking grazer can be done like this:

```grazer my_problem_data.json```

Calling grazer without arguments is equivalent to calling:

``` grazer problem_data.json```



## Feedback
Feedback is welcome! Just open a github issue or write an email to

e.fokken+grazer@posteo.de





