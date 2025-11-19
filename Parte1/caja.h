#ifndef CAJA_H
#define CAJA_H

#include "particula.h"

class Caja
{
public:
    //atributos
    double ancho, alto;

    //constructor
    Caja(double ancho, double alto);

    //metodos
    void verificarColision(Particula &p);
};

#endif // CAJA_H
