#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "Vector2D.h"
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

class Obstacle
{
private:
    Vector2D position;
    double width, height;
    double resistance;
    double maxResistance;
    int owner;
    QGraphicsRectItem* graphicsItem;
    QGraphicsTextItem* textItem;

public:
    Obstacle(Vector2D pos, double w, double h, double res, int owner);

    Vector2D getPosition() const;
    double getWidth() const;
    double getHeight() const;
    double getResistance() const;
    double getMaxResistance() const;
    int getOwner() const;
    bool isDestroyed() const;

    void setGraphicsItem(QGraphicsRectItem* item);
    void setTextItem(QGraphicsTextItem* item);
    QGraphicsRectItem* getGraphicsItem();
    QGraphicsTextItem* getTextItem();

    void takeDamage(double damage);
    bool checkCollision(Vector2D projPos, double projRadius, Vector2D& normal) const;
};

#endif
