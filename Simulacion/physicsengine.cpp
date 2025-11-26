#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine(double w, double h) : width(w), height(h) {}

void PhysicsEngine::updateProjectile(Projectile* proj, double dt)
{
    if (!proj->isActive()) return;

    proj->update(dt);

    // Verificar colisiones con paredes
    Vector2D pos = proj->getPosition();
    double r = proj->getRadius();

    // Pared izquierda y derecha
    if (pos.x - r < 0 || pos.x + r > width)
    {
        proj->bounceWall(true);
        proj->incrementBounce();
        // Corregir posición
        if (pos.x - r < 0) pos.x = r;
        if (pos.x + r > width) pos.x = width - r;
        proj->setPosition(pos);
    }

    // Pared superior e inferior
    if (pos.y - r < 0 || pos.y + r > height)
    {
        proj->bounceWall(false);
        proj->incrementBounce();
        // Corregir posición
        if (pos.y - r < 0) pos.y = r;
        if (pos.y + r > height) pos.y = height - r;
        proj->setPosition(pos);
    }
}

bool PhysicsEngine::checkObstacleCollision(Projectile* proj,
                                           std::vector<Obstacle*>& obstacles)
{
    if (!proj->isActive()) return false;

    Vector2D pos = proj->getPosition();
    double r = proj->getRadius();

    for (auto& obs : obstacles)
    {
        if (obs->isDestroyed()) continue;

        Vector2D normal;
        if (obs->checkCollision(pos, r, normal))
        {
            proj->incrementBounce();
            // Calcular daño
            double momentum = proj->getMomentum();
            double damage = DAMAGE_FACTOR * momentum;
            obs->takeDamage(damage);

            // Aplicar colisión inelástica
            Vector2D vel = proj->getVelocity();

            // Descomponer velocidad en normal y tangencial
            double velNormal = vel.dot(normal);
            Vector2D velNormalVec = normal * velNormal;
            Vector2D velTangent = vel - velNormalVec;

            // Aplicar coeficiente de restitución a componente normal
            velNormalVec = velNormalVec * (-RESTITUTION_COEFF);

            // Velocidad resultante
            Vector2D newVel = velNormalVec + velTangent;
            proj->setVelocity(newVel);

            // Alejar proyectil del obstáculo
            proj->setPosition(pos + normal * (r + 1));

            return true;
        }
    }
    return false;
}
