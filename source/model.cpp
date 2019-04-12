#include "model.h"

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
        _colors.push_back(QVector3D(1.0f, 1.0f, 1.0f));
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

    _graph.calculateGlueTags(_verticesGT, _indicesGT, _colorsGT);
    _graph.lines(_lineVertices, _lineColors);

    createGLModelContext();
}

void Model::createGLModelContext()
{

    _program->bind();

    createBuffers(_vao, _vbo, _ibo, _vertices, _indices, _colors);
    createBuffers(_vaoGT, _vboGT, _iboGT, _verticesGT, _indicesGT, _colorsGT);
    createBuffers(_vaoLines, _vboLines, _lineVertices, _lineColors);

    _program->release();
}

void Model::createBuffers(QOpenGLVertexArrayObject& vao, QOpenGLBuffer vbo[], QOpenGLBuffer& ibo, std::vector<QVector3D> vertices, std::vector<GLushort> indices, std::vector<QVector3D> colors)
{
    // delcare Vertex and Index buffer
    vbo[0] = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo[1] = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    ibo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);

    // create and bind VAO
    vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&vao);

    // get openglfunctions from the current context (important OGLWidget needs to call makeCurrent)
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    // create vbo for vertices
    vbo[0].create();
    vbo[0].bind();
    vbo[0].allocate(vertices.data(), int(vertices.size() * sizeof(QVector3D)));
    vbo[0].setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    vbo[0].release();

    // create vbo for colors
    vbo[1].create();
    vbo[1].bind();
    vbo[1].allocate(colors.data(), int(colors.size() * sizeof(QVector3D)));
    vbo[1].setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    vbo[1].release();

    // create IBO and allocate buffer
    ibo.create();
    ibo.bind();
    ibo.allocate(indices.data(), int(indices.size() * sizeof(GLushort)));
    ibo.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

    vaoBinder.release();
    ibo.release();

    vbo[0].destroy();
    vbo[1].destroy();
    ibo.destroy();
}

void Model::createBuffers(QOpenGLVertexArrayObject& vao, QOpenGLBuffer vbo[], std::vector<QVector3D> vertices, std::vector<QVector3D> colors)
{
    // delcare Vertex
    vbo[0] = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo[1] = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);

    // get openglfunctions from the current context (important OGLWidget needs to call makeCurrent)
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    vao.create();
    QOpenGLVertexArrayObject::Binder vaoLinesBinder(&vao);

    // create vbo for vertices
    vbo[0].create();
    vbo[0].bind();
    vbo[0].allocate(vertices.data(), int(vertices.size() * sizeof(QVector3D)));
    vbo[0].setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    vbo[0].release();

    // create vbo for colors
    vbo[1].create();
    vbo[1].bind();
    vbo[1].allocate(colors.data(), int(colors.size() * sizeof(QVector3D)));
    vbo[1].setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    vbo[1].release();
    vaoLinesBinder.release();
}

void Model::draw()
{
    // set the model Matrix
    _program->setUniformValue(_program->uniformLocation("modelMatrix"), _modelMatrix);

    // draw faces only if wireframe mode is not activated
    if(!_wireframe)
    {
        drawPolygons(_vao, _indices.size(), 1);
    }

    if(_showgluetags)
    {
        drawPolygons(_vaoGT, _indicesGT.size(), -1);
    }

    drawLines(_vaoLines);
}

void Model::drawLines(QOpenGLVertexArrayObject& vao)
{
    // get opengl functions
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    // bind the VAO
    QOpenGLVertexArrayObject::Binder vaoLinesBinder(&vao);
    // draw all lines
    f->glPolygonOffset(-1, -1);
    f->glDrawArrays(GL_LINES, 0, GLsizei(_lineVertices.size()));
    vaoLinesBinder.release();
}

void Model::drawPolygons(QOpenGLVertexArrayObject& vao, unsigned long triangles, float factor)
{
    // get opengl functions
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    // bind the VAO
    QOpenGLVertexArrayObject::Binder vaoBinder(&vao);
    // draw the solids
    f->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    f->glPolygonOffset(factor, factor);
    f->glEnable(GL_POLYGON_OFFSET_FILL);
    f->glDrawElements(GL_TRIANGLES, GLsizei(triangles), GL_UNSIGNED_SHORT, nullptr);
    f->glDisable(GL_POLYGON_OFFSET_FILL);
    vaoBinder.release();
}

void Model::showGluetags()
{
    _showgluetags = !_showgluetags;
}

void Model::switchRenderMode()
{
    _wireframe = !_wireframe;
}

Model::Model(QOpenGLShaderProgram* program)
{
    _program = program;
    createGLModelContext();
}

Model::~Model()
{
}
