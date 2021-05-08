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

```grazer path/to/my/problem_data.json```

Calling grazer without arguments is equivalent to calling:

``` grazer problem_data.json```

A simple example for integrating a single pipeline is found in `data/one_pipeline/`

### Structure of the input files.
The file `problem_data.json` has special structure and details what kind of problem types are present. Up to now only Networkproblems can be solved.
`problem_data.json` contains path names to different files, usually named `topology.json`, `boundary.json`, `initial.json` and `control.json`.
These detail the underlying graph of the problem to be solved and have similar structure, namely:

```json
{
    "nodes": {
        "Nodetype1": [
        {}
        ],
        "Nodetype2": [
        {},
        {}
        ]
    },
    "connections":{
        "Edgetype1":[
            {},
            {}
        ]
    }
}
```
where the empty brackets contain information on the nodes and edges of the respective type. What information can or must be given, depends on the component type in question, but all of them need and "id", that must be unique among all components.

### Structure of the boundary, initial and control values
The files `boundary.json`, `initial.json` and `control.json` have fixed structure. in them the empty brackets of above all look like this:
```json
{
    "id": "<component_name>",
    "data": [
        {
            "<label>": 0,
            "values": [
                60.3
            ]
        },
                {
            "<label>": 1500,
            "values": [
                58.9
            ]
        },
        {
            "<label>": 2000,
            "values": [
                52.6
            ]
        }
    ]
}
```
where `<label>` is either `"time"` (in `boundary.json` and `control.json` or `"x"` in `initial.json`. Also the number of values may differ.


### Output
The output will be a single json file in the directory `output` in the current working directory.
If a directory named `output` is already present, that directory will be renamed to `output_<some-big-number>`, where
<some-big-number> is given as the time in milliseconds since 01.01.1970.

The json is structured as follows:

## Feedback
Feedback is welcome! Just open a github issue or write an email to

e.fokken+grazer@posteo.de





