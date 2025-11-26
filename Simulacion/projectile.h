#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "GameObject.h"
#include <QGraphicsEllipseItem>

class Projectile : public GameObject
{
private:
    double radius;
    int owner;
    bool active;
    int bounceCount;
    QGraphicsEllipseItem* graphicsItem;

    static constexpr double g = 9.81;

public:
    Projectile(Vector2D pos, Vector2D vel, double m, double r, int owner);

    double getRadius() const;
    int getOwner() const;
    bool isActive() const;
    void deactivate();
    int getBounceCount() const;
    void incrementBounce();

    void setGraphicsItem(QGraphicsEllipseItem* item);
    QGraphicsEllipseItem* getGraphicsItem();

    void update(double dt) override;
    void bounceWall(bool isVertical);
};

#endif
