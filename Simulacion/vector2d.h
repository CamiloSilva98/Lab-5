#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <cmath>

class Vector2D
{
public:
    double x, y;

    Vector2D(double x = 0, double y = 0);

    double magnitude() const;
    Vector2D normalized() const;
    double dot(const Vector2D& other) const;

    Vector2D operator+(const Vector2D& other) const;
    Vector2D operator-(const Vector2D& other) const;
    Vector2D operator*(double scalar) const;
};

#endif
