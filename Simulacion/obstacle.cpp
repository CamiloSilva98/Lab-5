#include "Obstacle.h"
#include <algorithm>
#include <cmath>

Obstacle::Obstacle(Vector2D pos, double w, double h, double res, int owner)
    : position(pos), width(w), height(h), resistance(res),
    maxResistance(res), owner(owner), graphicsItem(nullptr),
    textItem(nullptr) {}

Vector2D Obstacle::getPosition() const
{
    return position;
}

double Obstacle::getWidth() const
{
    return width;
}

double Obstacle::getHeight() const
{
    return height;
}

double Obstacle::getResistance() const
{
    return resistance;
}

double Obstacle::getMaxResistance() const
{
    return maxResistance;
}

int Obstacle::getOwner() const
{
    return owner;
}

bool Obstacle::isDestroyed() const
{
    return resistance <= 0;
}

void Obstacle::setGraphicsItem(QGraphicsRectItem* item)
{
    graphicsItem = item;
}

void Obstacle::setTextItem(QGraphicsTextItem* item)
{
    textItem = item;
}

QGraphicsRectItem* Obstacle::getGraphicsItem()
{
    return graphicsItem;
}

QGraphicsTextItem* Obstacle::getTextItem()
{
    return textItem;
}

void Obstacle::takeDamage(double damage)
{
    resistance -= damage;
    if (resistance < 0) resistance = 0;
}

bool Obstacle::checkCollision(Vector2D projPos, double projRadius,
                              Vector2D& normal) const
{
    // Encontrar el punto más cercano del rectángulo al círculo
    double closestX = std::max(position.x,
                               std::min(projPos.x, position.x + width));
    double closestY = std::max(position.y,
                               std::min(projPos.y, position.y + height));

    double distX = projPos.x - closestX;
    double distY = projPos.y - closestY;
    double distSquared = distX * distX + distY * distY;

    if (distSquared < projRadius * projRadius)
    {
        // Determinar qué lado fue golpeado
        double left = std::abs(projPos.x - position.x);
        double right = std::abs(projPos.x - (position.x + width));
        double top = std::abs(projPos.y - position.y);
        double bottom = std::abs(projPos.y - (position.y + height));

        double minDist = std::min({left, right, top, bottom});

        if (minDist == left) normal = Vector2D(-1, 0);
        else if (minDist == right) normal = Vector2D(1, 0);
        else if (minDist == top) normal = Vector2D(0, -1);
        else normal = Vector2D(0, 1);

        return true;
    }
    return false;
}
