#ifndef PARTICULA_H
#define PARTICULA_H

class Particula
{
public:
    // Atributos
    double x, y;
    double vx, vy;
    double masa;
    double radio;
    bool activa;

    // Constructor
    Particula(double x = 0, double y = 0, double vx = 0, double vy = 0,
              double masa = 1, double radio = 1, bool activa = true);

    // Metodos
    void mover(double dt);
    bool colisionaCon(const Particula &p) const;
    void fusionar(const Particula &p);
    void reflejarParedX();
    void reflejarParedY();
    double energiaCinetica() const;
    double momentoLineal() const;

private:
    enum Lado { ARRIBA, ABAJO, IZQUIERDA, DERECHA, NINGUNO };
    Lado detectarLado(const Particula &p) const;
};
#endif // PARTICULA_H
