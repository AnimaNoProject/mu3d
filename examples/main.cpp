#include <iostream>
#include <../mu3d/include/graph.h>
#include <../mu3d/include/utility.hpp>

int main()
{
	mu3d::graph g = mu3d::graph();
	g.load("./assets/igloo.off");
	bool result = g.unfold(50000, 0);
	if (!result)
	{
		std::cerr << "failed to unfold." << std::endl;
	}
	else
	{
		//g.save("assets/model.obj", "assets/gluetabs.obj");
		g.save("assets/result.obj");
	}
	return 0;
}
