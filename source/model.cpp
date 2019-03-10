#include "model.h"

Model::Model(const char* filename)
{
    std::filebuf filebuffer;
    if(filebuffer.open(filename, std::ios::in))
    {
        std::cout << filename << std::endl;
        std::istream is(&filebuffer);
        CGAL::read_off(is, mesh);
        filebuffer.close();
    }

    CGAL::set_ascii_mode( std::cout);
    for ( Polyhedron::Vertex_iterator v = mesh.vertices_begin(); v != mesh.vertices_end(); ++v)
        std::cout << v->point() << std::endl;
}

Model::~Model()
{
}
