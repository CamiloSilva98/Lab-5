#include "simulacion.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

int main()
{
    // Inicializar generador de números aleatorios
    std::srand(std::time(nullptr));

    // Crear simulación con caja de 800x600
    simulacion sim(800, 600, 0.01);

    std::cout << "=== Configurando Simulacion ===\n";

    // Agregar 4 obstáculos cuadrados
    sim.agregarObstaculo(Obstaculo(200, 200, 50, 0.7));
    sim.agregarObstaculo(Obstaculo(550, 200, 50, 0.7));
    sim.agregarObstaculo(Obstaculo(200, 400, 50, 0.7));
    sim.agregarObstaculo(Obstaculo(550, 400, 50, 0.7));
    std::cout << "4 obstaculos agregados\n";

    // Agregar 6 partículas
    for (int i = 0; i < 6; i++)
    {
        double x = 50 + std::rand() % 700;
        double y = 50 + std::rand() % 500;
        double vx = -50 + std::rand() % 100;
        double vy = -50 + std::rand() % 100;
        double masa = 1.0 + (std::rand() % 5);
        double radio = 5 + (std::rand() % 10);
        sim.agregarParticula(Particula(x, y, vx, vy, masa, radio));
    }

    // Ejecutar simulación
    std::cout << "\n=== Ejecutando Simulacion ===\n";
    sim.simular(5.0, "trayectorias.txt");

    return 0;
}
