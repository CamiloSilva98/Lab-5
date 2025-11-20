#include "Vector2D.h"

Vector2D::Vector2D(double x, double y) : x(x), y(y) {}

double Vector2D::magnitude() const
{
    return std::sqrt(x * x + y * y);
}

Vector2D Vector2D::normalized() const
{
    double mag = magnitude();
    return mag > 0 ? Vector2D(x / mag, y / mag) : Vector2D(0, 0);
}

double Vector2D::dot(const Vector2D& other) const
{
    return x * other.x + y * other.y;
}

Vector2D Vector2D::operator+(const Vector2D& other) const
{
    return Vector2D(x + other.x, y + other.y);
}

Vector2D Vector2D::operator-(const Vector2D& other) const
{
    return Vector2D(x - other.x, y - other.y);
}

Vector2D Vector2D::operator*(double scalar) const
{
    return Vector2D(x * scalar, y * scalar);
}
