#include "Caja.h"

// Constructor
Caja::Caja(double ancho, double alto)
    : ancho(ancho), alto(alto) {}

// Verifica y maneja colisiones elásticas con las paredes
void Caja::verificarColision(Particula &p) {
    if (!p.activa) return;

    // Pared izquierda (x = 0)
    if (p.x - p.radio <= 0) {
        p.x = p.radio;  // Reposicionar
        p.reflejarParedX();
    }

    // Pared derecha (x = ancho)
    if (p.x + p.radio >= ancho) {
        p.x = ancho - p.radio;  // Reposicionar
        p.reflejarParedX();
    }

    // Pared superior (y = 0)
    if (p.y - p.radio <= 0) {
        p.y = p.radio;  // Reposicionar
        p.reflejarParedY();
    }

    // Pared inferior (y = alto)
    if (p.y + p.radio >= alto) {
        p.y = alto - p.radio;  // Reposicionar
        p.reflejarParedY();
    }
}
