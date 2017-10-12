


# Linear Photonics Graph Purification

This code is an efficient implementation of the path finding algorithm proposed in <http://www.google.com/>.

This software uses CMake as a build environment.
Thus the requirements to compile this software are:
- CMake v. 2.8
- c++ 11 compatible compiler (only tested with gcc)


## Compilation
1. `mkdir build`
2. `cd build`
3. `cmake ../`
4. `make`

## Running the program

The program generates a Raussendorf Lattice using the procedure described in [this paper](http://dx.doi.org/10.1103/PhysRevLett.115.020502). For the purification, we devide this lattice into boxes that correspond to one node in the purified lattice each.
```
Usage: ./src/main boxsize Nbox failure_rate seed
```

- **boxsize:** edge size (in nodes) of the boxes out of which the complete lattice is composed. For the Raussendorf lattice, this has to be an odd number.
- **Nbox:** How many boxes are there in one direction? Calculating `boxsize*Nbox` will result in the total number of nodes in each direction. (In time direction we only use 3 boxes)
- **failure_rate:** This is the failure rate for the Fusion gates and is required for the generation of the lattice. The recent fusion operations proposed either [here](https://doi.org/10.1103/PhysRevA.84.042331) or [here](http://dx.doi.org/10.1103/PhysRevLett.113.140403), have a failure rate of 25%.
- **seed:** The seed to the random number generator, that is needed for the generation of the latitce.


## Visualization

For small system sizes one can use blender for visualization.
### Usage of the Blender plugin:
In the folder `blender_plugin` there is a python script that can be used by Blender.
There are to methods to use the script in Blender. One can install it globally, or use it as on a per session basis:

- To install it globally, just follow the section *install the addon* in the following address: <https://docs.blender.org/api/blender_python_api_2_64_9/info_tutorial_addon.html#install-the-addon>

- To use the script on a per session basis open the blender file in the `blender_plugin` directory. Once loaded, open the text editor in blender, where the code should already be opened. Run the script from this window to activate it for this session.


After the blender script has been registered one can press space in the 3D view. A search window appears and one can search for `generate GraphState`. This should open a window where a json file can be selected and imported. This file is just one of the outputs of the main program. Again a warning: Blender loads all data into memory and tries to visualize it. **Thus, only small sizes are possible to visualize.**


## Data

The plots in the paper were done using the scripts found in the `data` folder.
Furtheremore there is also the raw data for the figures.