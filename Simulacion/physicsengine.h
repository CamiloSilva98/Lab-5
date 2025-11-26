#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "Projectile.h"
#include "Obstacle.h"
#include <vector>

class PhysicsEngine
{
private:
    double width, height;
    static constexpr double RESTITUTION_COEFF = 0.5;
    static constexpr double DAMAGE_FACTOR = 0.5;

public:
    PhysicsEngine(double w, double h);

    void updateProjectile(Projectile* proj, double dt);
    bool checkObstacleCollision(Projectile* proj, std::vector<Obstacle*>& obstacles);
};

#endif
