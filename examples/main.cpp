#include <iostream>
#include <../mu3d/include/graph.h>

int main()
{
	mu3d::graph g = mu3d::graph();
	g.load("./assets/igloo.off");
	bool result = g.unfold(100000, 1000);
	if (!result)
	{
		std::cerr << "failed to unfold." << std::endl;
	}
	else
	{
		g.save("assets/model.obj", "assets/gluetabs.obj");
	}
	return 0;
}
