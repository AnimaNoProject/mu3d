#include <iostream>
#include <../mu3d/include/graph.h>

int main()
{
	graph g = graph();
	g.load("C:\\Users\\thors\\Desktop\\octa.off");
	bool result = g.optimise(1000, 1000);
	if (!result)
	{
		std::cerr << "failed to unfold." << std::endl;
	}
	else
	{
		g.save("C:\\Users\\thors\\Desktop\\model.obj", "C:\\Users\\thors\\Desktop\\gluetabs.obj");
	}
	return 0;
}
