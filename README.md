# mu3d - 3D Meshunfolding Library

<!-- ABOUT THE PROJECT -->
## About The Project

Library to unfold triangulated 3D meshes with genus 0.
The meshes need to be provided in the .off file format and will be
saved in the .obj format. The code is based on the paper [Simulated Annealing to Unfold 3D Meshes and Assign Glue Tabs](https://otik.uk.zcu.cz/handle/11025/38424).

### Built With

* [glm](https://glm.g-truc.net/0.9.9/index.html)
* [CGAL](https://www.cgal.org/)

<!-- GETTING STARTED -->
## Usage

```C++
// initialize a graph object
mu3d::graph g = mu3d::graph();
// load a mesh
g.load("assets/octa.off");
// try to unfold the mesh within 1000 iterations
// further, 1000 iterations to optimise the unfolding if one is found
bool result = g.unfold(1000, 1000);
if (!result)
{
    std::cerr << "Failed to unfold within 1000 iterations." << std::endl;
}
else
{
    g.save("assets/model.obj", "assets/gluetabs.obj");
}
return 0;
```

<!-- PREREQUISITES -->
### Prerequisites

To be able to build the library you need to have CGAL installed.
