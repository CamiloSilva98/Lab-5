#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    loadSprites();
    setupUI();
    setupGame();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::gameLoop);
    timer->start(16);
}

MainWindow::~MainWindow() {
    delete gameManager;
}
void MainWindow::loadSprites()
{
    // Cargar sprites del techo (4 estados: 100%, 75%, 50%, 25%)
    roofSprites[0].load(":/sprits/sprites/blocksH1.png"); // 100% vida
    roofSprites[1].load(":/sprits/sprites/blocksH2.png"); // 75% vida (usa la misma por ahora)
    roofSprites[2].load(":/sprits/sprites/blocksH3.png"); // 50% vida
    roofSprites[3].load(":/sprits/sprites/blocksH4.png"); // 25% vida

    // Cargar sprites de los muros laterales (3 estados: 100%, 66%, 33%)
    wallSprites[0].load(":/sprits/sprites/blocks1.png"); // 100% vida
    wallSprites[1].load(":/sprits/sprites/blocks2.png"); // 66% vida
    wallSprites[2].load(":/sprits/sprites/blocks3.png"); // 33% vida

    // Cargar sprite del centro
    centerSprite.load(":/sprits/sprites/centro.png");
}

QPixmap MainWindow::getSpriteForObstacle(Obstacle* obs) {
    double healthPercent = obs->getHealthPercentage();
    Vector2D pos = obs->getPosition();

    // Determinar tipo de obstáculo por posición/tamaño
    bool isRoof = (obs->getHeight() == 50);     // Techo: altura 50
    bool isCenter = (obs->getWidth() == 80);    // Centro: ancho 80
    bool isWall = !isRoof && !isCenter;         // Muros: el resto

    QPixmap sprite;

    if (isRoof) {
        // TECHO: 4 estados según vida
        if (healthPercent > 75) {
            sprite = roofSprites[0]; // 100-75%
        } else if (healthPercent > 50) {
            sprite = roofSprites[1]; // 75-50%
        } else if (healthPercent > 25) {
            sprite = roofSprites[2]; // 50-25%
        } else {
            sprite = roofSprites[3]; // 25-0%
        }
    }
    else if (isWall) {
        // MUROS: 3 estados según vida
        if (healthPercent > 66) {
            sprite = wallSprites[0]; // 100-66%
        } else if (healthPercent > 33) {
            sprite = wallSprites[1]; // 66-33%
        } else {
            sprite = wallSprites[2]; // 33-0%
        }
    }
    else {
        sprite = centerSprite;
    }

    // Escalar al tamaño del obstáculo
    return sprite.scaled(obs->getWidth(), obs->getHeight(),
                         Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}
void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    scene = new QGraphicsScene(0, 0, ARENA_WIDTH, ARENA_HEIGHT, this);
    QPixmap backgroundImage(":/sprits/sprites/fondo1.png");

    if (!backgroundImage.isNull())
    {
        // Si la imagen existe, usarla como fondo
        QBrush backgroundBrush(backgroundImage.scaled(ARENA_WIDTH, ARENA_HEIGHT,
                        Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        scene->setBackgroundBrush(backgroundBrush);

    } else {
        // Si no existe, usar color sólido
        scene->setBackgroundBrush(QColor(135, 206, 235)); // Azul cielo
    }

    // Agregar borde al campo de juego
    scene->addRect(0, 0, ARENA_WIDTH, ARENA_HEIGHT, QPen(Qt::black, 3));

    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    mainLayout->addWidget(view);

    QHBoxLayout* controlLayout = new QHBoxLayout();

    playerLabel = new QLabel("Turno: Jugador 1", this);
    QFont labelFont = playerLabel->font();
    labelFont.setPointSize(11);
    labelFont.setBold(true);
    playerLabel->setFont(labelFont);
    controlLayout->addWidget(playerLabel);

    bouncesLabel = new QLabel("Rebotes: 0/6", this);
    bouncesLabel->setFont(labelFont);
    bouncesLabel->setStyleSheet("QLabel { font-weight: bold; color: #333; }");
    controlLayout->addWidget(bouncesLabel);

    controlLayout->addWidget(new QLabel("Ángulo:", this));
    angleSlider = new QSlider(Qt::Horizontal, this);
    angleSlider->setRange(-25, 90);
    angleSlider->setValue(45);
    controlLayout->addWidget(angleSlider);

    angleLabel = new QLabel("45°", this);
    controlLayout->addWidget(angleLabel);

    controlLayout->addWidget(new QLabel("Velocidad:", this));
    speedSlider = new QSlider(Qt::Horizontal, this);
    speedSlider->setRange(15, 150);
    speedSlider->setValue(30);
    controlLayout->addWidget(speedSlider);

    speedLabel = new QLabel("50 m/s", this);
    controlLayout->addWidget(speedLabel);

    launchButton = new QPushButton("🚀 Lanzar", this);
    launchButton->setStyleSheet("QPushButton { padding: 8px 20px; font-weight: bold; }");
    controlLayout->addWidget(launchButton);

    mainLayout->addLayout(controlLayout);

    connect(angleSlider, &QSlider::valueChanged, this, &MainWindow::onAngleChanged);
    connect(speedSlider, &QSlider::valueChanged, this, &MainWindow::onSpeedChanged);
    connect(launchButton, &QPushButton::clicked, this, &MainWindow::onLaunchClicked);

    resize(850, 750);
    setWindowTitle("Juego de Estrategia Militar");
}

void MainWindow::setupGame() {
    gameManager = new GameManager(ARENA_WIDTH, ARENA_HEIGHT);

    // JUGADOR 1 (Izquierda) - Estructura en forma de fortaleza
    // Techo (superior)
    gameManager->addObstacle(new Obstacle(Vector2D(50, 400), 240, 50, 1000, 1));
    // Lateral izquierdo
    gameManager->addObstacle(new Obstacle(Vector2D(50, 450), 50, 150, 200, 1));
    // Centro con "Rival"
    gameManager->addObstacle(new Obstacle(Vector2D(130, 520), 80, 80, 50, 1));
    // Lateral derecho
    gameManager->addObstacle(new Obstacle(Vector2D(240, 450), 50, 150, 300, 1));

    // JUGADOR 2 (Derecha) - Estructura en forma de fortaleza
    // Techo (superior)
    gameManager->addObstacle(new Obstacle(Vector2D(510, 400), 240, 50, 1000, 2));
    // Lateral izquierdo
    gameManager->addObstacle(new Obstacle(Vector2D(510, 450), 50, 150, 300, 2));
    // Centro con "Rival"
    gameManager->addObstacle(new Obstacle(Vector2D(590, 520), 80, 80, 50, 2));
    // Lateral derecho
    gameManager->addObstacle(new Obstacle(Vector2D(700, 450), 50, 150, 200, 2));

    for (auto obs : gameManager->getObstacles())
    {
        Vector2D pos = obs->getPosition();

        // Obtener sprite según tipo y vida
        QPixmap sprite = getSpriteForObstacle(obs);

        // Crear item gráfico con el sprite
        QGraphicsPixmapItem* spriteItem = scene->addPixmap(sprite);
        spriteItem->setPos(pos.x, pos.y);
        obs->setSpriteItem(spriteItem);
    }
    // Dibujar bases de cañones en las esquinas superiores
    cannonBase1 = scene->addEllipse(10, 180, 40, 40,
                                    QPen(Qt::black, 3), QBrush(Qt::darkBlue));
    cannonBase2 = scene->addEllipse(ARENA_WIDTH - 50, 180, 40, 40,
                                    QPen(Qt::black, 3), QBrush(Qt::darkRed));

    // Crear cañones
    QPen cannonPen(Qt::black, 5);
    cannonPen.setCapStyle(Qt::RoundCap);

    cannon1 = scene->addLine(0, 0, 0, 0, cannonPen);
    cannon1->setZValue(10);

    cannon2 = scene->addLine(0, 0, 0, 0, cannonPen);
    cannon2->setZValue(10);

    updateCannons();
}

void MainWindow::gameLoop() {
    gameManager->update(DT);
    updateGraphics();

    int winner = gameManager->checkVictory();
    if (winner != 0) {
        timer->stop();
        QMessageBox::information(this, "¡Fin del Juego!",
                                 QString("¡Jugador %1 ha ganado!").arg(winner));
    }
}

void MainWindow::updateGraphics() {
    // Actualizar obstáculos
    for (auto obs : gameManager->getObstacles()) {
        // Actualizar texto de resistencia
        if (obs->getTextItem()) {
            obs->getTextItem()->setPlainText(
                QString::number((int)obs->getResistance())
                );
        }

        // Actualizar sprite según vida restante
        if (obs->getSpriteItem() && !obs->isDestroyed())
        {
            QPixmap newSprite = getSpriteForObstacle(obs);
            obs->getSpriteItem()->setPixmap(newSprite);
        }

        // Ocultar si está destruido
        if (obs->isDestroyed())
        {
            if (obs->getSpriteItem()) obs->getSpriteItem()->setVisible(false);
            if (obs->getTextItem()) obs->getTextItem()->setVisible(false);
        }
    }

    // Actualizar proyectil
    Projectile* proj = gameManager->getCurrentProjectile();
    if (proj && gameManager->isProjectileInFlight()) {
        if (!proj->getGraphicsItem()) {
            QGraphicsEllipseItem* item = scene->addEllipse(
                0, 0, proj->getRadius() * 2, proj->getRadius() * 2,
                QPen(Qt::black),
                QBrush(proj->getOwner() == 1 ? Qt::darkBlue : Qt::darkRed)
                );
            proj->setGraphicsItem(item);
        }

        Vector2D pos = proj->getPosition();
        proj->getGraphicsItem()->setPos(
            pos.x - proj->getRadius(),
            pos.y - proj->getRadius()
            );

        // Actualizar contador de rebotes
        int bounces = proj->getBounceCount();
        int maxBounces = gameManager->getMaxBounces();
        bouncesLabel->setText(QString("Rebotes: %1/%2").arg(bounces).arg(maxBounces));

        if (bounces >= maxBounces - 1) {
            bouncesLabel->setStyleSheet("QLabel { font-weight: bold; color: red; }");
        }
        else
        {
            bouncesLabel->setStyleSheet("QLabel { font-weight: bold; color: #333; }");
        }
    }
    else
    {
        QList<QGraphicsItem*> items = scene->items();
        for (QGraphicsItem* item : items) {
            QGraphicsEllipseItem* ellipse = dynamic_cast<QGraphicsEllipseItem*>(item);
            if (ellipse && ellipse != cannonBase1 && ellipse != cannonBase2) {
                // Es un proyectil (no es base de cañón)
                scene->removeItem(ellipse);
                delete ellipse;
            }
        }
        bouncesLabel->setText("Rebotes: 0/5");
        bouncesLabel->setStyleSheet("QLabel { font-weight: bold; color: #333; }");
    }

    playerLabel->setText(
        QString("Turno: Jugador %1").arg(gameManager->getCurrentPlayer())
        );

    launchButton->setEnabled(!gameManager->isProjectileInFlight());

    if (!gameManager->isProjectileInFlight()) {
        updateCannons();
    }
}

void MainWindow::onLaunchClicked() {
    double angle = angleSlider->value();
    double speed = speedSlider->value();
    gameManager->launchProjectile(angle, speed);
}

void MainWindow::onAngleChanged(int value) {
    angleLabel->setText(QString("%1°").arg(value));
    if (!gameManager->isProjectileInFlight()) {
        updateCannons(); // Actualizar cañón en tiempo real
    }
}

void MainWindow::onSpeedChanged(int value) {
    speedLabel->setText(QString("%1 m/s").arg(value));
}

void MainWindow::updateCannons() {
    double angle = angleSlider->value();
    double angleRad = angle * M_PI / 180.0;

    // Posición del centro de cada cañón (esquinas superiores)
    double player1X = 30;
    double player1Y = 200;

    double player2X = ARENA_WIDTH - 30;
    double player2Y = 200;

    // Calcular punto final del cañón para jugador 1 (dispara a la derecha y hacia abajo)
    double endX1 = player1X + CANNON_LENGTH * std::cos(angleRad);
    double endY1 = player1Y + CANNON_LENGTH * std::sin(angleRad);  // Cambiado a + para disparar hacia abajo

    // Calcular punto final del cañón para jugador 2 (dispara a la izquierda y hacia abajo)
    double endX2 = player2X - CANNON_LENGTH * std::cos(angleRad);
    double endY2 = player2Y + CANNON_LENGTH * std::sin(angleRad);  // Cambiado a + para disparar hacia abajo

    // Actualizar líneas de cañones
    cannon1->setLine(player1X, player1Y, endX1, endY1);
    cannon2->setLine(player2X, player2Y, endX2, endY2);

    // Resaltar el cañón del jugador actual
    if (gameManager->getCurrentPlayer() == 1) {
        cannon1->setPen(QPen(Qt::darkBlue, 5, Qt::SolidLine, Qt::RoundCap));
        cannon2->setPen(QPen(Qt::darkGray, 3, Qt::SolidLine, Qt::RoundCap));
    } else {
        cannon1->setPen(QPen(Qt::darkGray, 3, Qt::SolidLine, Qt::RoundCap));
        cannon2->setPen(QPen(Qt::darkRed, 5, Qt::SolidLine, Qt::RoundCap));
    }
}
