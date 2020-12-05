#include <iostream>
#include <../mu3d/include/graph.h>

int main()
{
	mu3d::graph g = mu3d::graph();
	g.load("./assets/igloo_complex.off");
	bool result = g.unfold(100000, 0);
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
