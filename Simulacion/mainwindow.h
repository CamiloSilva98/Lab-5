#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
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
    QPushButton* launchButton;

    static constexpr double ARENA_WIDTH = 800;
    static constexpr double ARENA_HEIGHT = 600;
    static constexpr double DT = 0.016;

    void setupUI();
    void setupGame();
    void updateGraphics();

private slots:
    void gameLoop();
    void onLaunchClicked();
    void onAngleChanged(int value);
    void onSpeedChanged(int value);

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
};

#endif
