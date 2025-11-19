#ifndef OBSTACULO_H
#define OBSTACULO_H

#include "particula.h"

class Obstaculo
{
public:
    //atributos
    double x, y, lado, e;

    //constructor
    Obstaculo(double x, double y, double lado, double e);

    //metodos
    bool colisionaCon(Particula &p);
    void aplicarRebote(Particula &p);
};

#endif // OBSTACULO_H
