#include "camera.h"

Camera::Camera() : _delta(-10), _angleX(0), _angleY(0)
{
}

Camera::~Camera()
{
}

QMatrix4x4 Camera::getMatrix()
{
    // calculate the view Matrix and return it
    QMatrix4x4 matrix;
    matrix.setToIdentity();
    matrix.translate(0, 0, _delta);
    matrix.rotate(_angleX, 0, 1, 0);
    matrix.rotate(_angleY, 1, 0, 0);
    return matrix;
}

void Camera::reset()
{
    _delta = -10;
    _angleX = 0;
    _angleY = 0;
}

void Camera::rotate(float angleX, float angleY)
{
    _angleX += angleX;
    _angleY += angleY;
}

void Camera::zoom(float delta)
{
    _delta += delta;
}
