#pragma once
#include <QMatrix4x4>

class Camera
{
public:
    Camera();
    ~Camera();
    QMatrix4x4 getMatrix();
    void rotate(float angleX, float angleY);
    void zoom(float delta);
private:
    float _delta;
    float _angleX;
    float _angleY;
    const float _speed = 0.45f;
};
