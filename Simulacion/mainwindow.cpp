#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUI();
    setupGame();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::gameLoop);
    timer->start(16);
}

MainWindow::~MainWindow() {
    delete gameManager;
}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    scene = new QGraphicsScene(0, 0, ARENA_WIDTH, ARENA_HEIGHT, this);
    scene->setBackgroundBrush(QColor(240, 240, 240)); // Fondo gris más claro

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

    bouncesLabel = new QLabel("Rebotes: 0/5", this);
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
    gameManager->addObstacle(new Obstacle(Vector2D(70, 220), 200, 50, 100, 1));
    // Lateral izquierdo
    gameManager->addObstacle(new Obstacle(Vector2D(70, 270), 60, 150, 200, 1));
    // Centro con "Rival"
    gameManager->addObstacle(new Obstacle(Vector2D(130, 270), 80, 150, 100, 1));
    // Lateral derecho
    gameManager->addObstacle(new Obstacle(Vector2D(210, 270), 60, 150, 200, 1));

    // JUGADOR 2 (Derecha) - Estructura en forma de fortaleza
    // Techo (superior)
    gameManager->addObstacle(new Obstacle(Vector2D(530, 220), 200, 50, 100, 2));
    // Lateral izquierdo
    gameManager->addObstacle(new Obstacle(Vector2D(530, 270), 60, 150, 200, 2));
    // Centro con "Rival"
    gameManager->addObstacle(new Obstacle(Vector2D(590, 270), 80, 150, 100, 2));
    // Lateral derecho
    gameManager->addObstacle(new Obstacle(Vector2D(670, 270), 60, 150, 200, 2));

    // Dibujar obstáculos
    for (auto obs : gameManager->getObstacles()) {
        Vector2D pos = obs->getPosition();

        // Determinar color según resistencia
        QColor color;
        if (obs->getResistance() == 200) {
            color = obs->getOwner() == 1 ? QColor(255, 200, 100) : QColor(255, 200, 100); // Naranjo claro
        } else {
            color = QColor(220, 220, 220); // Gris claro para techo y centro
        }

        QGraphicsRectItem* rect = scene->addRect(
            pos.x, pos.y, obs->getWidth(), obs->getHeight(),
            QPen(Qt::black, 2),
            QBrush(color)
            );
        obs->setGraphicsItem(rect);

        // Texto de resistencia
        QGraphicsTextItem* text = scene->addText(
            QString::number((int)obs->getResistance())
            );
        QFont font = text->font();
        font.setPointSize(14);
        font.setBold(true);
        text->setFont(font);

        // Centrar texto en el obstáculo
        double textX = pos.x + (obs->getWidth() - text->boundingRect().width()) / 2;
        double textY = pos.y + (obs->getHeight() - text->boundingRect().height()) / 2;
        text->setPos(textX, textY);
        obs->setTextItem(text);

        // Agregar etiqueta "Rival" en el centro
        if (obs->getResistance() == 100 && obs->getWidth() == 80) {
            QGraphicsTextItem* rivalText = scene->addText("Rival");
            QFont rivalFont = rivalText->font();
            rivalFont.setPointSize(10);
            rivalFont.setBold(true);
            rivalText->setFont(rivalFont);
            double rivalX = pos.x + (obs->getWidth() - rivalText->boundingRect().width()) / 2;
            double rivalY = pos.y + obs->getHeight() - 25;
            rivalText->setPos(rivalX, rivalY);
        }
    }

    // Dibujar bases de cañones en las esquinas superiores
    cannonBase1 = scene->addEllipse(10, 10, 40, 40,
                                    QPen(Qt::black, 3), QBrush(Qt::darkBlue));
    cannonBase2 = scene->addEllipse(ARENA_WIDTH - 50, 10, 40, 40,
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
    for (auto obs : gameManager->getObstacles()) {
        if (obs->getTextItem()) {
            obs->getTextItem()->setPlainText(
                QString::number((int)obs->getResistance())
                );
        }

        if (obs->isDestroyed() && obs->getGraphicsItem()) {
            obs->getGraphicsItem()->setVisible(false);
            obs->getTextItem()->setVisible(false);
        }
    }

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

        // Cambiar color si está cerca del límite
        if (bounces >= maxBounces - 1) {
            bouncesLabel->setStyleSheet("QLabel { font-weight: bold; color: red; }");
        } else {
            bouncesLabel->setStyleSheet("QLabel { font-weight: bold; color: #333; }");
        }
    } else {
        if (proj && proj->getGraphicsItem()) {
            scene->removeItem(proj->getGraphicsItem());
            delete proj->getGraphicsItem();
            proj->setGraphicsItem(nullptr);
        }
        bouncesLabel->setText("Rebotes: 0/3");
        bouncesLabel->setStyleSheet("QLabel { font-weight: bold; color: #333; }");
    }

    playerLabel->setText(
        QString("Turno: Jugador %1").arg(gameManager->getCurrentPlayer())
        );

    launchButton->setEnabled(!gameManager->isProjectileInFlight());

    // Actualizar cañones solo cuando no hay proyectil en vuelo
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
    double player1Y = 30;

    double player2X = ARENA_WIDTH - 30;
    double player2Y = 30;

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
