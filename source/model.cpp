#include "model.h"

// default cube size
const float width = 4;
const float height = 4;
const float depth = 4;

// default cube vertices
const std::vector<QVector3D> defaultCubeVertices = {
    // front
    QVector3D(-width / 2.0f, -height / 2.0f,  depth / 2.0f),
    QVector3D(width / 2.0f, -height / 2.0f,  depth / 2.0f),
    QVector3D(width / 2.0f,  height / 2.0f,  depth / 2.0f),
    QVector3D(-width / 2.0f,  height / 2.0f,  depth / 2.0f),
    // back
    QVector3D(-width / 2.0f, -height / 2.0f, -depth / 2.0f),
    QVector3D(width / 2.0f, -height / 2.0f, -depth / 2.0f),
    QVector3D(width / 2.0f,  height / 2.0f, -depth / 2.0f),
    QVector3D(-width / 2.0f,  height / 2.0f, -depth / 2.0f)
};

// default cube indices
const std::vector<GLushort> defaultCubeIndices = {// front
                                0, 1, 2,
                                2, 3, 0,
                                // top
                                1, 5, 6,
                                6, 2, 1,
                                // back
                                7, 6, 5,
                                5, 4, 7,
                                // bottom
                                4, 0, 3,
                                3, 7, 4,
                                // left
                                4, 5, 1,
                                1, 0, 4,
                                // right
                                3, 2, 6,
                                6, 7, 3};

Model::Model(const char* filename, QOpenGLShaderProgram* program)
{
    _program = program;

    // open file buffer
    std::filebuf filebuffer;
    if(filebuffer.open(filename, std::ios::in))
    {
        // read .off file into CGAL::Polyhedron_3
        std::istream is(&filebuffer);
        CGAL::read_off(is, _mesh);
        filebuffer.close();
    }

    QVector3D middle(0,0,0);

    // get all vertices
    for ( Polyhedron::Vertex_iterator v = _mesh.vertices_begin(); v != _mesh.vertices_end(); ++v)
    {
        _vertices.push_back(QVector3D(float(v->point().x()), float(v->point().y()), float(v->point().z())));
        middle += QVector3D(float(v->point().x())/_mesh.size_of_vertices(),
                            float(v->point().y())/_mesh.size_of_vertices(),
                            float(v->point().z())/_mesh.size_of_vertices());
    }

    // loop through all facets
    for (Polyhedron::Facet_iterator fi = _mesh.facets_begin(); fi != _mesh.facets_end(); ++fi)
    {
        _graph.addFace(fi);

        // get the first facet
        Polyhedron::Halfedge_around_facet_circulator hfc = fi->facet_begin();
        // assert that all facets are triangles
        CGAL_assertion(CGAL::circulator_size(hfc) >= 3);
        do
        {
            unsigned short foundindex = 0;
            // loop through vertices and save their index if they match
            for(std::vector<int>::size_type i = 0; i != _vertices.size(); ++i)
            {
                if(float(hfc->vertex()->point().x()) == _vertices[i].x() &&
                   float(hfc->vertex()->point().y()) == _vertices[i].y() &&
                   float(hfc->vertex()->point().z()) == _vertices[i].z())
                {
                    foundindex = ushort(i);
                    break;
                }
            }
            _indices.push_back(foundindex);
        } while (++hfc != fi->facet_begin());
    }

    _modelMatrix.setToIdentity();
    _modelMatrix.translate(QVector3D(0,0,0) - middle);

    _graph.calculateDual();
    _graph.calculateMSP();
    _graph.getMSPVertices(_mspVertices);

    createGLModelContext();
}

void Model::createGLModelContext()
{
    // delcare Vertex and Index buffer
    _vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    _ibo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    _program->bind();

    // create and bind VAO
    _vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);

    // get openglfunctions from the current context (important OGLWidget needs to call makeCurrent)
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    // create VBO and allocate buffer
    _vbo.create();
    _vbo.bind();
    _vbo.allocate(_vertices.data(), int(_vertices.size() * sizeof(QVector3D)));
    _vbo.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    _vbo.release();

    // create IBO and allocate buffer
    _ibo.create();
    _ibo.bind();
    _ibo.allocate(_indices.data(), int(_indices.size() * sizeof(GLushort)));
    _ibo.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

    vaoBinder.release();
    _ibo.release();

    QOpenGLVertexArrayObject::Binder vaoDualBinder(&_vaoDual);
    _vboMSP.create();
    _vboMSP.bind();
    _vboMSP.allocate(_mspVertices.data(), int(_mspVertices.size() * sizeof(QVector3D)));
    _vboMSP.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    _vboMSP.release();
    vaoDualBinder.release();

    _program->release();
}

void Model::draw()
{
    // get opengl functions
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();
    // bind the VAO
    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);
    // set the model Matrix
    _program->setUniformValue(_program->uniformLocation("modelMatrix"), _modelMatrix);
    // draw all triangles
    if(_wireframe)
    {
        _program->setUniformValue(_program->uniformLocation("color"), _lineColor);
        f->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        f->glDrawElements(GL_TRIANGLES, GLsizei(_indices.size()), GL_UNSIGNED_SHORT, nullptr);
    }
    else
    {
        // draw the solids
        _program->setUniformValue(_program->uniformLocation("color"), _fillColor);
        f->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        f->glPolygonOffset(1, 1);
        f->glEnable(GL_POLYGON_OFFSET_FILL);
        f->glDrawElements(GL_TRIANGLES, GLsizei(_indices.size()), GL_UNSIGNED_SHORT, nullptr);
        f->glDisable(GL_POLYGON_OFFSET_FILL);
        // draw the lines
        _program->setUniformValue(_program->uniformLocation("color"), _lineColor);
        f->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        f->glLineWidth(10);
        f->glDrawElements(GL_TRIANGLES, GLsizei(_indices.size()), GL_UNSIGNED_SHORT, nullptr);
    }

    QOpenGLVertexArrayObject::Binder vaoDualBinder(&_vaoDual);
    _program->setUniformValue(_program->uniformLocation("color"), _mspColor);
    f->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    f->glPolygonOffset(-1, -1);
    f->glDrawArrays(GL_LINES, 0, GLsizei(_mspVertices.size()));
}

void Model::switchRenderMode()
{
    _wireframe = !_wireframe;
}

Model::Model(QOpenGLShaderProgram* program)
{
    _program = program;

    // if no .off file is specified use standard cube to draw
    _vertices = defaultCubeVertices;
    _indices = defaultCubeIndices;
    _modelMatrix.setToIdentity();

    createGLModelContext();
}


Model::~Model()
{
}
