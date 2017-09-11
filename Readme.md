


# Linear Photonics Graph Purification

This code is an efficient implementation of the path finding algorithm proposed in TODO.

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

The program currently takes two parameters: the edge size for the smaller boxes and how many boxes there are in one direction.

```
./src/main 8 5
```

## Visualization

There are two approaches to visualization, one using blender and one using javascript. Both cannot visualize large amounts of data, but blender seems to be a bit more stable. To scale up, blender and different render layers can be used or a point cloud visualization package should be used.
### Usage of the blender plugin:
Either register the script as a plugin for blender and activate it, so that it gets loaded automatically, or copy and paste the script into the blender editor and register it from there.
Then, while 3D view is selected press space and type `generate GraphState`. This should open a window where a json file can be selected and imported.

### Usage of the javascript package

Just open the index.html file in your favourite browser, that supports javascript. It automatically loads `../ymeasure.json`.