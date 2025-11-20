#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    setupUI();
    setupGame();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::gameLoop);
    timer->start(16);
}

MainWindow::~MainWindow()
{
    delete gameManager;
}

void MainWindow::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    scene = new QGraphicsScene(0, 0, ARENA_WIDTH, ARENA_HEIGHT, this);
    scene->setBackgroundBrush(Qt::lightGray);

    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    mainLayout->addWidget(view);

    QHBoxLayout* controlLayout = new QHBoxLayout();

    playerLabel = new QLabel("Turno: Jugador 1", this);
    controlLayout->addWidget(playerLabel);

    controlLayout->addWidget(new QLabel("Ángulo:", this));
    angleSlider = new QSlider(Qt::Horizontal, this);
    angleSlider->setRange(0, 90);
    angleSlider->setValue(45);
    controlLayout->addWidget(angleSlider);

    angleLabel = new QLabel("45°", this);
    controlLayout->addWidget(angleLabel);

    controlLayout->addWidget(new QLabel("Velocidad:", this));
    speedSlider = new QSlider(Qt::Horizontal, this);
    speedSlider->setRange(10, 100);
    speedSlider->setValue(50);
    controlLayout->addWidget(speedSlider);

    speedLabel = new QLabel("50 m/s", this);
    controlLayout->addWidget(speedLabel);

    launchButton = new QPushButton("Lanzar", this);
    controlLayout->addWidget(launchButton);

    mainLayout->addLayout(controlLayout);

    connect(angleSlider, &QSlider::valueChanged, this, &MainWindow::onAngleChanged);
    connect(speedSlider, &QSlider::valueChanged, this, &MainWindow::onSpeedChanged);
    connect(launchButton, &QPushButton::clicked, this, &MainWindow::onLaunchClicked);

    resize(850, 750);
    setWindowTitle("Juego de Estrategia Militar");
}

void MainWindow::setupGame()
{
    gameManager = new GameManager(ARENA_WIDTH, ARENA_HEIGHT);

    gameManager->addObstacle(new Obstacle(Vector2D(150, 400), 60, 80, 200, 1));
    gameManager->addObstacle(new Obstacle(Vector2D(150, 250), 60, 80, 200, 1));
    gameManager->addObstacle(new Obstacle(Vector2D(250, 325), 60, 80, 150, 1));

    gameManager->addObstacle(new Obstacle(Vector2D(590, 400), 60, 80, 200, 2));
    gameManager->addObstacle(new Obstacle(Vector2D(590, 250), 60, 80, 200, 2));
    gameManager->addObstacle(new Obstacle(Vector2D(490, 325), 60, 80, 150, 2));

    for (auto obs : gameManager->getObstacles())
    {
        Vector2D pos = obs->getPosition();
        QGraphicsRectItem* rect = scene->addRect(
            pos.x, pos.y, obs->getWidth(), obs->getHeight(),
            QPen(Qt::black, 2),
            obs->getOwner() == 1 ? QBrush(Qt::blue) : QBrush(Qt::red)
            );
        obs->setGraphicsItem(rect);

        QGraphicsTextItem* text = scene->addText(
            QString::number((int)obs->getResistance())
            );
        text->setPos(pos.x + 15, pos.y + 30);
        obs->setTextItem(text);
    }

    scene->addEllipse(35, ARENA_HEIGHT - 65, 30, 30,
                      QPen(Qt::black), QBrush(Qt::darkBlue));
    scene->addEllipse(ARENA_WIDTH - 65, ARENA_HEIGHT - 65, 30, 30,
                      QPen(Qt::black), QBrush(Qt::darkRed));
}

void MainWindow::gameLoop()
{
    gameManager->update(DT);
    updateGraphics();

    int winner = gameManager->checkVictory();
    if (winner != 0)
    {
        timer->stop();
        QMessageBox::information(this, "¡Fin del Juego!",
                                 QString("¡Jugador %1 ha ganado!").arg(winner));
    }
}

void MainWindow::updateGraphics()
{
    for (auto obs : gameManager->getObstacles())
    {
        if (obs->getTextItem())
        {
            obs->getTextItem()->setPlainText(
                QString::number((int)obs->getResistance())
                );
        }

        if (obs->isDestroyed() && obs->getGraphicsItem())
        {
            obs->getGraphicsItem()->setVisible(false);
            obs->getTextItem()->setVisible(false);
        }
    }

    Projectile* proj = gameManager->getCurrentProjectile();
    if (proj && gameManager->isProjectileInFlight())
    {
        if (!proj->getGraphicsItem())
        {
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
    } else if (proj && proj->getGraphicsItem())
    {
        scene->removeItem(proj->getGraphicsItem());
        delete proj->getGraphicsItem();
        proj->setGraphicsItem(nullptr);
    }

    playerLabel->setText(
        QString("Turno: Jugador %1").arg(gameManager->getCurrentPlayer())
        );

    launchButton->setEnabled(!gameManager->isProjectileInFlight());
}

void MainWindow::onLaunchClicked()
{
    double angle = angleSlider->value();
    double speed = speedSlider->value();
    gameManager->launchProjectile(angle, speed);
}

void MainWindow::onAngleChanged(int value)
{
    angleLabel->setText(QString("%1°").arg(value));
}

void MainWindow::onSpeedChanged(int value)
{
    speedLabel->setText(QString("%1 m/s").arg(value));
}
