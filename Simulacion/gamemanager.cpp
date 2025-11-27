#include "GameManager.h"
#include <cmath>

GameManager::GameManager(double arenaWidth, double arenaHeight)
    : currentPlayer(1), currentProjectile(nullptr),
    projectileInFlight(false), projectileTimer(0.0),
    player1Pos(30, 200),  // Esquina superior izquierda
    player2Pos(770, 200) {  // Esquina superior derecha
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

    // Ajustar dirección según el jugador (desde esquinas superiores hacia abajo)
    double direction = (currentPlayer == 1) ? 1.0 : -1.0;
    Vector2D velocity(direction * speed * std::cos(angleRad),
                      speed * std::sin(angleRad));  // Positivo para disparar hacia abajo

    currentProjectile = new Projectile(launchPos, velocity,
                                       1.0, 5.0, currentPlayer);
    projectileInFlight = true;
    projectileTimer = 0.0; // Resetear el cronómetro
}

void GameManager::update(double dt) {
    if (!projectileInFlight || !currentProjectile) return;

    projectileTimer += dt;

    physics->updateProjectile(currentProjectile, dt);
    physics->checkObstacleCollision(currentProjectile, obstacles);

    // Verificar condiciones para terminar el turno
    Vector2D vel = currentProjectile->getVelocity();
    int bounces = currentProjectile->getBounceCount();

    // Condición 1: Velocidad muy baja
    bool velocityTooLow = vel.magnitude() < 3.0;

    // Condición 2: Máximo de rebotes alcanzado
    bool maxBouncesReached = bounces >= MAX_BOUNCES;

    // Condición 3: Tiempo máximo
    bool timeExpired = projectileTimer > 100.0;

    if (velocityTooLow || maxBouncesReached || timeExpired) {
        endTurn();
    }
}

void GameManager::endTurn()
{
    if (currentProjectile && currentProjectile->getGraphicsItem())
    projectileInFlight = false;
    if (currentProjectile) {
        delete currentProjectile;
        currentProjectile = nullptr;
    }
    currentPlayer = (currentPlayer == 1) ? 2 : 1;
}

int GameManager::checkVictory() {
    bool player1CenterDestroyed = false;
    bool player2CenterDestroyed = false;

    for (auto obs : obstacles)
    {
        if (obs->getMaxResistance() == 50)
        {
            if (obs->getOwner() == 1 && obs->isDestroyed())
            {
                player1CenterDestroyed = true;
            }
            if (obs->getOwner() == 2 && obs->isDestroyed())
            {
                player2CenterDestroyed = true;
            }
        }
    }

    // Jugador 2 gana si destruye el centro de Jugador 1
    if (player1CenterDestroyed) return 2;

    // Jugador 1 gana si destruye el centro de Jugador 2
    if (player2CenterDestroyed) return 1;

    // Nadie ha ganado aún
    return 0;
}

