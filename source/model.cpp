#include "model.h"

Model::Model(const char* filename)
{
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
        _vertices.push_back(Utility::pointToVector(v->point()));
        middle += (Utility::pointToVector(v->point()) / _mesh.size_of_vertices());
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
                if(Utility::pointToVector(hfc->vertex()->point()) == _vertices[i])
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

    _graph.initializeState();

    _graph.oglGluetags(_verticesGT, _indicesGT, _colorsGT);
    _graph.oglLines(_lineVertices, _lineColors);
}

int Model::finishedAnnealing()
{
    if (_graph.energy() <= 0 || _graph.over())
    {
        std::cout << "Energy: " << _graph.energy() << " and " << _graph.over() << std::endl;
        return 0;
    }
    else
        return int(_graph.temperature);
}

bool Model::finishedBruteForce()
{
    return _graph.energy() <= 0 || _graph.finishedBruteFroce();
}

void Model::clearGL()
{
    clearOGL();
}

void Model::load3DGL(QOpenGLShaderProgram* program)
{
    _graph.oglGluetags(_verticesGT, _indicesGT, _colorsGT);
    _graph.oglLines(_lineVertices, _lineColors);

    program->bind();
    Utility::createBuffers(_vaoGT, _vboGT, _iboGT, _verticesGT, _indicesGT, _colorsGT);
    Utility::createBuffers(_vaoLines, _vboLines, _lineVertices, _lineColors);
    Utility::createBuffers(_vao, _vbo, _ibo, _vertices, _indices, _colors);
    program->release();

    _vbo[0].destroy();
    _vbo[1].destroy();
    _vboGT[0].destroy();
    _vboGT[1].destroy();
    _vboLines[0].destroy();
    _vboLines[1].destroy();
}

bool Model::bruteForce()
{
    return _graph.nextBruteForce();
}

bool Model::anneal()
{
    return _graph.neighbourState();
}

void Model::createGLModelContext(QOpenGLShaderProgram* program)
{    
    program->bind();
    Utility::createBuffers(_vao, _vbo, _ibo, _vertices, _indices, _colors);
    Utility::createBuffers(_vaoGT, _vboGT, _iboGT, _verticesGT, _indicesGT, _colorsGT);
    Utility::createBuffers(_vaoLines, _vboLines, _lineVertices, _lineColors);
    program->release();

    _vbo[0].destroy();
    _vbo[1].destroy();
    _vboGT[0].destroy();
    _vboGT[1].destroy();
    _vboLines[0].destroy();
    _vboLines[1].destroy();
}

void Model::loadPlanarGL(QOpenGLShaderProgram* program)
{
    _graph.oglPlanar(_planarVertices, _planarColors, _planarLines, _planarLinesColors, _modelMatrixPlanar);

    program->bind();
    Utility::createBuffers(_vaoPlanar, _vboPlanar, _planarVertices, _planarColors);
    Utility::createBuffers(_vaoPlanarLines, _vboPlanarLines, _planarLines, _planarLinesColors);
    program->release();

    _vboPlanar[0].destroy();
    _vboPlanar[1].destroy();
    _vboPlanarLines[0].destroy();
    _vboPlanarLines[1].destroy();
}

void Model::clearOGL()
{
    _lineVertices.clear();
    _lineColors.clear();

    _planarVertices.clear();
    _planarColors.clear();
    _planarLines.clear();
    _planarLinesColors.clear();

    _vao.destroy();
    _ibo.destroy();

    _vaoGT.destroy();
    _iboGT.destroy();

    _verticesGT.clear();
    _indicesGT.clear();
    _colorsGT.clear();

    _vaoLines.destroy();

    _vaoPlanar.destroy();

    _vaoPlanarLines.destroy();
}

void Model::draw(QOpenGLShaderProgram* program)
{
    // set the model Matrix
    program->setUniformValue(program->uniformLocation("modelMatrix"), _modelMatrix);

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

void Model::drawPolygons(QOpenGLVertexArrayObject& vao, unsigned long triangles, float offset)
{
    // get opengl functions
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    // bind the VAO
    QOpenGLVertexArrayObject::Binder vaoBinder(&vao);
    // draw the solids
    f->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    f->glPolygonOffset(offset, offset);
    f->glEnable(GL_POLYGON_OFFSET_FILL);
    f->glDrawElements(GL_TRIANGLES, GLsizei(triangles), GL_UNSIGNED_SHORT, nullptr);
    f->glDisable(GL_POLYGON_OFFSET_FILL);
    vaoBinder.release();
}

void Model::drawPlanarPatch(QOpenGLShaderProgram* program)
{
    // set the model Matrix
    program->setUniformValue(program->uniformLocation("modelMatrix"), _modelMatrixPlanar);

    // get opengl functions
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    // bind the VAO
    QOpenGLVertexArrayObject::Binder vaoBinder(&_vaoPlanar);
    // draw the solids
    f->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    f->glPolygonOffset(1, 1);
    f->glEnable(GL_POLYGON_OFFSET_FILL);
    f->glDrawArrays(GL_TRIANGLES, 0, GLsizei(_planarVertices.size()));
    f->glDisable(GL_POLYGON_OFFSET_FILL);
    vaoBinder.release();

    QOpenGLVertexArrayObject::Binder vaoLinesBinder(&_vaoPlanarLines);
    // draw all lines
    f->glPolygonOffset(-1, -1);
    f->glDrawArrays(GL_LINES, 0, GLsizei(_planarLines.size()));
    vaoLinesBinder.release();
}

void Model::showGluetags()
{
    _showgluetags = !_showgluetags;
}

void Model::switchRenderMode()
{
    _wireframe = !_wireframe;
}

Model::Model()
{
}

Model::~Model()
{
    delete this;
}
