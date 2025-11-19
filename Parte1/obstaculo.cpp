
#include "obstaculo.h"
#include <cmath>
#include <algorithm>

// Constructor
Obstaculo::Obstaculo(double x, double y, double lado, double e)
    : x(x), y(y), lado(lado), e(e) {}

// Verifica si la partícula colisiona con el obstáculo
bool Obstaculo::colisionaCon(Particula &p) {
    if (!p.activa) return false;

    // Encuentra el punto más cercano del rectángulo al centro del círculo
    double cercanoX = std::max(x, std::min(p.x, x + lado));
    double cercanoY = std::max(y, std::min(p.y, y + lado));

    // Calcula la distancia del punto más cercano al centro del círculo
    double dx = p.x - cercanoX;
    double dy = p.y - cercanoY;
    double distancia = std::sqrt(dx * dx + dy * dy);

    return distancia < p.radio;
}

// Aplica rebote inelástico con coeficiente de restitución
void Obstaculo::aplicarRebote(Particula &p) {
    if (!p.activa) return;

    // Centro del obstáculo
    double centroObsX = x + lado / 2.0;
    double centroObsY = y + lado / 2.0;

    // Vector del centro del obstáculo a la partícula
    double dx = p.x - centroObsX;
    double dy = p.y - centroObsY;

    // Determinar qué lado es más probable (el más cercano)
    // Calculamos las distancias a cada borde
    double dist_izq = p.x - x;
    double dist_der = (x + lado) - p.x;
    double dist_arr = p.y - y;
    double dist_aba = (y + lado) - p.y;

    // Encontrar el mínimo
    double min_dist = std::min({dist_izq, dist_der, dist_arr, dist_aba});

    // Determinar lado de colisión basado en la distancia más corta
    if (min_dist == dist_izq) {
        // Colisión con lado izquierdo
        p.x = x - p.radio - 0.1;  // Pequeño margen extra
        p.vx = -e * std::abs(p.vx);  // Velocidad hacia la izquierda
    }
    else if (min_dist == dist_der) {
        // Colisión con lado derecho
        p.x = x + lado + p.radio + 0.1;
        p.vx = e * std::abs(p.vx);  // Velocidad hacia la derecha
    }
    else if (min_dist == dist_arr) {
        // Colisión con lado superior
        p.y = y - p.radio - 0.1;
        p.vy = -e * std::abs(p.vy);  // Velocidad hacia arriba
    }
    else {
        // Colisión con lado inferior
        p.y = y + lado + p.radio + 0.1;
        p.vy = e * std::abs(p.vy);  // Velocidad hacia abajo
    }

    // MÉTODO ALTERNATIVO (más robusto para esquinas)
    // Si la partícula está cerca de una esquina, usar método de normal
    bool cerca_esquina = false;

    // Verificar si está en esquina
    if ((std::abs(dx) / lado > 0.35) && (std::abs(dy) / lado > 0.35)) {
        cerca_esquina = true;
    }

    if (cerca_esquina) {
        // Para esquinas, reflejar en dirección normal
        double dist = std::sqrt(dx * dx + dy * dy);
        if (dist > 0.001) {
            double nx = dx / dist;  // Normal unitaria
            double ny = dy / dist;

            // Componente de velocidad en dirección normal
            double vn = p.vx * nx + p.vy * ny;

            // Solo reflejar si se está moviendo hacia el obstáculo
            if (vn < 0) {
                // Reflejar componente normal, mantener componente tangencial
                p.vx = p.vx - (1 + e) * vn * nx;
                p.vy = p.vy - (1 + e) * vn * ny;
            }

            // Reposicionar fuera del obstáculo
            double empuje = p.radio + lado * 0.71 + 1.0;  // 0.71 ≈ sqrt(2)/2
            p.x = centroObsX + nx * empuje;
            p.y = centroObsY + ny * empuje;
        }
    }
}
