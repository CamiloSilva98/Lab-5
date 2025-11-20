#include "GameManager.h"
#include <cmath>

GameManager::GameManager(double arenaWidth, double arenaHeight)
    : currentPlayer(1), currentProjectile(nullptr),
    projectileInFlight(false),
    player1Pos(50, arenaHeight - 50),
    player2Pos(arenaWidth - 50, arenaHeight - 50) {
    physics = new PhysicsEngine(arenaWidth, arenaHeight);
}

GameManager::~GameManager() {
    delete physics;
    if (currentProjectile) delete currentProjectile;
    for (auto obs : obstacles) delete obs;
}

void GameManager::addObstacle(Obstacle* obs) {
    obstacles.push_back(obs);
}

std::vector<Obstacle*>& GameManager::getObstacles() {
    return obstacles;
}

int GameManager::getCurrentPlayer() const {
    return currentPlayer;
}

bool GameManager::isProjectileInFlight() const {
    return projectileInFlight;
}

Projectile* GameManager::getCurrentProjectile() {
    return currentProjectile;
}

Vector2D GameManager::getLaunchPosition() const {
    return currentPlayer == 1 ? player1Pos : player2Pos;
}

void GameManager::launchProjectile(double angle, double speed) {
    if (projectileInFlight) return;

    Vector2D launchPos = getLaunchPosition();

    // Convertir ángulo a radianes y calcular velocidad inicial
    double angleRad = angle * M_PI / 180.0;

    // Ajustar dirección según el jugador
    double direction = (currentPlayer == 1) ? 1.0 : -1.0;
    Vector2D velocity(direction * speed * std::cos(angleRad),
                      -speed * std::sin(angleRad));

    currentProjectile = new Projectile(launchPos, velocity,
                                       1.0, 5.0, currentPlayer);
    projectileInFlight = true;
}

void GameManager::update(double dt) {
    if (!projectileInFlight || !currentProjectile) return;

    physics->updateProjectile(currentProjectile, dt);
    physics->checkObstacleCollision(currentProjectile, obstacles);

    // Verificar si el proyectil tiene velocidad muy baja
    Vector2D vel = currentProjectile->getVelocity();

    if (vel.magnitude() < 5.0) {
        endTurn();
    }
}

void GameManager::endTurn() {
    projectileInFlight = false;
    if (currentProjectile) {
        delete currentProjectile;
        currentProjectile = nullptr;
    }
    currentPlayer = (currentPlayer == 1) ? 2 : 1;
}

int GameManager::checkVictory() {
    int player1Obstacles = 0, player2Obstacles = 0;

    for (auto obs : obstacles) {
        if (!obs->isDestroyed()) {
            if (obs->getOwner() == 1) player1Obstacles++;
            else player2Obstacles++;
        }
    }

    if (player1Obstacles == 0) return 2;
    if (player2Obstacles == 0) return 1;
    return 0;
}
