#ifndef SIMULACION_H
#define SIMULACION_H

#include <vector>
#include <fstream>
#include "particula.h"
#include "obstaculo.h"
#include "caja.h"

class simulacion
{
public:
    //Atributos
    std::vector<Particula> particulas;
    std::vector<Obstaculo> obstaculos;
    Caja caja;
    double dt;
    double tiempoTotal;

    //Constructor
    simulacion(double ancho, double alto, double dt=0.01);
    void agregarParticula(const Particula &p);
    void agregarObstaculo(const Obstaculo &o);
    void simular(double duracion, const std::string &archivo);

private:
    void actualizarPosiciones();
    void detectarColisiones();
    void manejarColisionParticulaObstaculo(Particula &p, Obstaculo &o);
    void manejarColisionEntreParticulas(Particula &p1, Particula &p2);
    void separarParticulas(Particula &p1, Particula &p2);
    void guardarEstados(std::ofstream &archivo, double tiempo);
    int contarParticulasActivas() const;
};

#endif // SIMULACION_H
