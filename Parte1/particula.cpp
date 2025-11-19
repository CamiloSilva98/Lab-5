#include "Particula.h"
#include <cmath>

// Constructor
Particula::Particula(double x, double y, double vx, double vy,
                     double masa, double radio, bool activa)
    : x(x), y(y), vx(vx), vy(vy), masa(masa), radio(radio), activa(activa) {}

// Actualiza la posición de la partícula según su velocidad
void Particula::mover(double dt) {
    if (!activa) return;
    x += vx * dt;
    y += vy * dt;
}

// Verifica si esta partícula colisiona con otra
bool Particula::colisionaCon(const Particula &p) const {
    if (!activa || !p.activa) return false;

    // Calcula la distancia entre centros
    double dx = x - p.x;
    double dy = y - p.y;
    double distancia = std::sqrt(dx * dx + dy * dy);

    // Hay colisión si la distancia es menor o igual a la suma de radios
    return distancia <= (radio + p.radio);
}

// Fusiona esta partícula con otra (colisión completamente inelástica)
void Particula::fusionar(const Particula &p) {
    if (!activa || !p.activa) return;

    // Guardar valores originales
    double x_orig = x;
    double y_orig = y;
    double masa_orig = masa;

    // Conservación del momento lineal
    double masa_total = masa + p.masa;

    // Nueva velocidad según conservación del momento
    vx = (masa * vx + p.masa * p.vx) / masa_total;
    vy = (masa * vy + p.masa * p.vy) / masa_total;

    // Nueva posición (centro de masa)
    x = (masa_orig * x_orig + p.masa * p.x) / masa_total;
    y = (masa_orig * y_orig + p.masa * p.y) / masa_total;

    // Nueva masa (suma de ambas masas)
    masa = masa_total;

    // Nuevo radio (conservando área: A = π*r²)
    radio = std::sqrt(radio * radio + p.radio * p.radio);
}

// Refleja la velocidad en el eje X (rebote elástico en paredes verticales)
void Particula::reflejarParedX() {
    if (!activa) return;
    vx = -vx;
}

// Refleja la velocidad en el eje Y (rebote elástico en paredes horizontales)
void Particula::reflejarParedY() {
    if (!activa) return;
    vy = -vy;
}

// Calcula la energía cinética de la partícula
double Particula::energiaCinetica() const {
    if (!activa) return 0;
    double v = std::sqrt(vx * vx + vy * vy);
    return 0.5 * masa * v * v;
}

// Calcula el momento lineal de la partícula
double Particula::momentoLineal() const {
    if (!activa) return 0;
    return masa * std::sqrt(vx * vx + vy * vy);
}

// Detecta con qué lado colisionó (método privado, no usado actualmente)
Particula::Lado Particula::detectarLado(const Particula &p) const {
    return NINGUNO;
}
