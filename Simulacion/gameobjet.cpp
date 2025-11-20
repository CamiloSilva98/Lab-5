#include "Gameobject.h"

GameObject::GameObject(Vector2D pos, Vector2D vel, double m)
    : position(pos), velocity(vel), mass(m) {}

GameObject::~GameObject() {}

Vector2D GameObject::getPosition() const
{
    return position;
}

Vector2D GameObject::getVelocity() const
{
    return velocity;
}

double GameObject::getMass() const
{
    return mass;
}

void GameObject::setPosition(Vector2D pos)
{
    position = pos;
}

void GameObject::setVelocity(Vector2D vel)
{
    velocity = vel;
}

void GameObject::update(double dt) {
    position = position + velocity * dt;
}

double GameObject::getMomentum() const
{
    return mass * velocity.magnitude();
}
