## Control Schema

- Is the key `connections` necessary? (No siblings)
- - Yes, because `nodes` may (and will) become controllable.

- It appears `Controlvalve` and `Compressorstation` have pretty much the same type of data. Should they be in the same list with a `controlType` string specifying whether it is a Controlvalve or Compressorstation?
- - That's a question up to debate. Sorting beforehand has the benefit of better performance at the cost of reduced ergonomics.
- why is there only one `time` but multiple `values` in the data? Is the value supposed to be the control value at a certain time? If yes, should therer not be a way to specify the different times as well?
- - One time point may get a number of values. This may never happen for controls, but the control json follows the same syntax as the boundary and initial value jsons, where this happens.


### Boundary Schema

similarly to control schema

- `nodes` has no siblings - necessary?
- - There will probably be `connections` that need boundary values.
- all the nodes have the same type of data -> merge them into list with "node_type" property?
- - Same as under control: performance vs. ergonomics. Need to talk about that.


### Topology Schema

- What meta information is necessary? (required)
- - Good question. It's probably a good idea to clone the GasLib xml schema for all gas type components. Will talk about that.
- some "nodes" are similar:
    - Source, Sink and Innode have the exact same properties - merge?
    - Vphinode, PVnode and PQnode have the same properties - merge?
    - in general: merge all nodes into a list of nodes with a "type" property?
- - Same as above.
- "height" does not have a unit but is an array with a value key, should a unit be required?
- - What??? Error in gaslib
- connections could also be a list with a `connection_type` property
- - siehe oben
- should x, y also have units? -> more similar structure?
- - What??? Error in gaslib

### Problem Data Schema

- Why is there a subkey "problem_data"?  Isn't the entire file already the "problem_data"?
- - Talk...
