#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Projectile.h"
#include "Obstacle.h"
#include "PhysicsEngine.h"
#include <vector>

class GameManager {
private:
    int currentPlayer;
    PhysicsEngine* physics;
    Projectile* currentProjectile;
    std::vector<Obstacle*> obstacles;
    bool projectileInFlight;
    double projectileTimer;

    static constexpr int MAX_BOUNCES = 6;

    Vector2D player1Pos;
    Vector2D player2Pos;

public:
    GameManager(double arenaWidth, double arenaHeight);
    ~GameManager();

    void addObstacle(Obstacle* obs);
    std::vector<Obstacle*>& getObstacles();



    int getCurrentPlayer() const;
    bool isProjectileInFlight() const;
    Projectile* getCurrentProjectile();
    Vector2D getLaunchPosition() const;

    void launchProjectile(double angle, double speed);
    void update(double dt);
    void endTurn();
    int checkVictory();
    int getMaxBounces() const { return MAX_BOUNCES; }
};

#endif
