#include <iostream>
#include <mu3d/graph.h>

int main()
{
	mu3d::graph g = mu3d::graph();
	g.load("./assets/igloo.off");
	bool result = g.unfold(20000, 0);
	if (!result)
	{
		std::cerr << "failed to unfold." << std::endl;
	}
	else
	{
		g.save("assets/model.obj", "assets/gluetabs.obj", "assets/mirrorgt.obj");
		std::cout << "yaaay" << std::endl;
	}
	return 0;
}
