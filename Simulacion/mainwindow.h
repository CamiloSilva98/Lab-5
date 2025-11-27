#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include "GameManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    QGraphicsScene* scene;
    QGraphicsView* view;
    GameManager* gameManager;
    QTimer* timer;

    QSlider* angleSlider;
    QSlider* speedSlider;
    QLabel* angleLabel;
    QLabel* speedLabel;
    QLabel* playerLabel;
    QLabel* bouncesLabel;
    QPushButton* launchButton;

    QPixmap roofSprites[4];   // 4 estados del techo
    QPixmap wallSprites[3];   // 3 estados de los muros
    QPixmap centerSprite;

    static constexpr double ARENA_WIDTH = 800;
    static constexpr double ARENA_HEIGHT = 600;
    static constexpr double DT = 0.048 ;
    static constexpr double CANNON_LENGTH = 40;

    QGraphicsLineItem* cannon1;
    QGraphicsLineItem* cannon2;
    QGraphicsEllipseItem* cannonBase1;
    QGraphicsEllipseItem* cannonBase2;

    void setupUI();
    void setupGame();
    void updateGraphics();
    void loadSprites();
    QPixmap getSpriteForObstacle(class Obstacle* obs);

private slots:
    void gameLoop();
    void onLaunchClicked();
    void onAngleChanged(int value);
    void onSpeedChanged(int value);
    void updateCannons();


public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

};

#endif
