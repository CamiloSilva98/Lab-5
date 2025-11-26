#include "Projectile.h"

Projectile::Projectile(Vector2D pos, Vector2D vel, double m, double r, int owner)
    : GameObject(pos, vel, m), radius(r), owner(owner),
    active(true), graphicsItem(nullptr) {}

double Projectile::getRadius() const
{
    return radius;
}

int Projectile::getOwner() const
{
    return owner;
}

bool Projectile::isActive() const
{
    return active;
}

void Projectile::deactivate()
{
    active = false;
}

int Projectile::getBounceCount() const
{
    return bounceCount;
}

void Projectile::incrementBounce()
{
    bounceCount++;
}

void Projectile::setGraphicsItem(QGraphicsEllipseItem* item)
{
    graphicsItem = item;
}

QGraphicsEllipseItem* Projectile::getGraphicsItem()
{
    return graphicsItem;
}

void Projectile::update(double dt)
{
    // Aplicar gravedad
    velocity.y += g * dt;

    // Actualizar posición
    position = position + velocity * dt + 0.5* g * dt * dt;
}

void Projectile::bounceWall(bool isVertical)
{
    if (isVertical)
    {
        velocity.x = -velocity.x;
    }
    else
    {
        velocity.y = -velocity.y;
    }
}
