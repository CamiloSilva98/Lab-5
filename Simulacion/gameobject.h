#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Vector2D.h"

class GameObject
{
protected:
    Vector2D position;
    Vector2D velocity;
    double mass;

public:
    GameObject(Vector2D pos, Vector2D vel, double m);
    virtual ~GameObject();

    Vector2D getPosition() const;
    Vector2D getVelocity() const;
    double getMass() const;

    void setPosition(Vector2D pos);
    void setVelocity(Vector2D vel);

    virtual void update(double dt);
    virtual double getMomentum() const;
};

#endif
