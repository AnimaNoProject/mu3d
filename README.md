# mu3d - 3D Meshunfolding Library

<!-- ABOUT THE PROJECT -->
## About The Project

Library to unfold triangulated 3D meshes with genus 0. The meshes need to be provided in the .off file format and will be saved in the .obj format. 
The code is based on the paper [Simulated Annealing to Unfold 3D Meshes and Assign Glue Tabs](https://otik.uk.zcu.cz/handle/11025/38424).
Native implementation for C++ and a wrapper for python is provided.

### Built With

* [glm](https://glm.g-truc.net/0.9.9/index.html)
* [CGAL](https://www.cgal.org/)
* [Boost](https://www.boost.org/)

<!-- GETTING STARTED -->
## Usage

### Native implementation

```C++
#include <iostream>
#include <mu3d/graph.h>
// initialize a graph object
mu3d::graph g = mu3d::graph();
// load a mesh
g.load("assets/octa.off");
// try to unfold the mesh within 10000 iterations
// further, 1000 iterations to optimise the unfolding if one is found
bool result = g.unfold(10000, 1000);
if (!result)
{
    std::cerr << "Failed to unfold within 10000 iterations." << std::endl;
}
else
{
    g.save("assets/igoo.obj");
}
return 0;
```

### Python wrapper

```python
from mu3dpy.mu3d import Graph

g = Graph()
g.load("igloo.off")

if not g.unfold(5000, 0):
    print("failed to unfold :(")
else:
    print("succesfully unfolded :)")
    g.save_single("temp.obj")
```

<!-- PREREQUISITES -->
### Prerequisites

To be able to build the library you need to have CGAL and Boost installed.
For the python wrapper to work the mu3d.dll needs to be placed in the folder containing the mu3d.py file.
Post build the mu3d.dll is copied to the mu3dpy folder, that contains the python wrapper.

* Install [CGAL](https://www.cgal.org/download/windows.html)
* Add `CGAL_DIR` to your environment variables pointing to your CGAL installation (i.e. `C:\dev\CGAL-5.1`)
* Install [Boost](https://www.boost.org/users/history/version_1_76_0.html)
* Add `BOOST_ROOT` to your environment variables poingto to your Boost installation (i.e. `C:\dev\boost_1_74_0`)
