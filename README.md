stl2swc
=======

This tools is meant to convert STL files to SWC files using _Mean Curcature
Skeletons_ as implemented in the CGAL library. At the moment though the tool
generates only a list of edges to represent a skelton, SWC support has yet to be
added.

Dependencies
------------

1. `CMake`
2. `CGAL`
3. `Eigen`

Building
--------

1. `mkdir build`
2. `cd build`
3. `cmake ..`
4. `make`
5. `./stl2swc <stl-file> [<num-subdiv-steps> [<quality-speed-tradeoff [<medially-centered-speed-tradeoff>]]]`

Parameters
----------

With the few experiment done so far, these seem like a useful set of parameters
for neuronal meshes:

`./stl2swc <stl-file> 1 0.3 0.4`

The defaults used without explicit parameters are provided by CGAL:

`./stl2swc <stl-file> 0 0.1 0.2`
