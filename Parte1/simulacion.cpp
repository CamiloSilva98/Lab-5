#include "simulacion.h"
#include <iostream>
#include <iomanip>
#include <cmath>

// Constructor
simulacion::simulacion(double ancho, double alto, double dt)
    : caja(ancho, alto), dt(dt), tiempoTotal(0) {}

// Agregar partícula al sistema
void simulacion::agregarParticula(const Particula &p) {
    particulas.push_back(p);
}

// Agregar obstáculo al sistema
void simulacion::agregarObstaculo(const Obstaculo &o)
{
    obstaculos.push_back(o);
}

// Contar partículas activas
int simulacion::contarParticulasActivas() const
{
    int count = 0;
    for (const auto &p : particulas)
    {
        if (p.activa) count++;
    }
    return count;
}

// Actualizar posiciones de todas las partículas
void simulacion::actualizarPosiciones()
{
    for (auto &p : particulas)
    {
        if (p.activa) {
            p.mover(dt);
            caja.verificarColision(p);
        }
    }
}

// Separar dos partículas que se están sobrelapando
void simulacion::separarParticulas(Particula &p1, Particula &p2) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    double dist = std::sqrt(dx * dx + dy * dy);

    if (dist < 0.001) {
        // Si están exactamente en el mismo lugar, separar en dirección aleatoria
        dx = 1.0;
        dy = 0.0;
        dist = 1.0;
    }

    // Normalizar
    dx /= dist;
    dy /= dist;

    // Distancia mínima necesaria
    double minDist = p1.radio + p2.radio;
    double overlap = minDist - dist;

    if (overlap > 0)
    {
        // Separar proporcionalmente a las masas (la más ligera se mueve más)
        double total_masa = p1.masa + p2.masa;
        double factor1 = p2.masa / total_masa;
        double factor2 = p1.masa / total_masa;

        p1.x -= dx * overlap * factor1 * 1.01; // 1.01 para asegurar separación
        p1.y -= dy * overlap * factor1 * 1.01;
        p2.x += dx * overlap * factor2 * 1.01;
        p2.y += dy * overlap * factor2 * 1.01;
    }
}

// Manejar colisión entre partícula y obstáculo
void simulacion::manejarColisionParticulaObstaculo(Particula &p, Obstaculo &o) {
    if (!p.activa) return;

    if (o.colisionaCon(p))
    {
        o.aplicarRebote(p);
        // Verificar que la partícula quedó fuera del obstáculo
        // Si no, hacer ajuste adicional
        if (o.colisionaCon(p)) {
            // Forzar separación adicional
            double centroObsX = o.x + o.lado / 2.0;
            double centroObsY = o.y + o.lado / 2.0;
            double dx = p.x - centroObsX;
            double dy = p.y - centroObsY;
            double dist = std::sqrt(dx * dx + dy * dy);

            if (dist > 0.001)
            {
                dx /= dist;
                dy /= dist;
                // Mover la partícula más lejos del obstáculo
                p.x = centroObsX + dx * (o.lado * 0.71 + p.radio + 1);
                p.y = centroObsY + dy * (o.lado * 0.71 + p.radio + 1);
            }
        }
    }
}

// Manejar colisión entre dos partículas (fusión)
void simulacion::manejarColisionEntreParticulas(Particula &p1, Particula &p2) {
    if (!p1.activa || !p2.activa) return;

    if (p1.colisionaCon(p2))
    {
        // Guardar energías antes de la fusión (para debugging)
        double energia_antes = p1.energiaCinetica() + p2.energiaCinetica();

        // Fusionar p2 en p1
        p1.fusionar(p2);
        p2.activa = false;

        // Calcular energía después
        double energia_despues = p1.energiaCinetica();
        std::cout << "Fusion detectada: "
                  << "E_antes=" << energia_antes
                  << " E_despues=" << energia_despues
                  << " Perdida=" << (100.0 * (1.0 - energia_despues/energia_antes)) << "%\n";
    }
}

// Detectar todas las colisiones
void simulacion::detectarColisiones() {
    // PASO 1: Colisiones partícula-obstáculo (primero para evitar que entren en obstáculos)
    for (auto &p : particulas) {
        if (!p.activa) continue;
        for (auto &o : obstaculos) {
            manejarColisionParticulaObstaculo(p, o);
        }
    }

    // PASO 2: Colisiones partícula-partícula
    for (size_t i = 0; i < particulas.size(); i++) {
        if (!particulas[i].activa) continue;

        for (size_t j = i + 1; j < particulas.size(); j++) {
            if (!particulas[j].activa) continue;

            // Verificar si están colisionando
            if (particulas[i].colisionaCon(particulas[j])) {
                // Fusionar
                manejarColisionEntreParticulas(particulas[i], particulas[j]);

                // Si la fusión ocurrió, ya no necesitamos verificar más con particulas[j]
                if (!particulas[j].activa) {
                    break; // Salir del loop interno
                }
            }
        }
    }
}

// Guardar estado actual en archivo
void simulacion::guardarEstados(std::ofstream &archivo, double tiempo)
{
    for (size_t i = 0; i < particulas.size(); i++)
    {
        if (particulas[i].activa)
        {
            archivo << std::fixed << std::setprecision(3)
            << tiempo << "\t"
            << i << "\t"
            << particulas[i].x << "\t"
            << particulas[i].y << "\t"
            << particulas[i].vx << "\t"
            << particulas[i].vy << "\t"
            << particulas[i].masa << "\t"
            << particulas[i].radio << "\n";
        }
    }
}

// Ejecutar simulación
void simulacion::simular(double duracion, const std::string &archivo) {
    std::ofstream salida(archivo);

    if (!salida.is_open()) {
        std::cerr << "Error al abrir archivo de salida\n";
        return;
    }

    // Escribir encabezado
    salida << "# Tiempo\tID\tX\tY\tVX\tVY\tMasa\tRadio\n";

    tiempoTotal = 0;
    int paso = 0;
    int pasos_por_segundo = static_cast<int>(1.0 / dt);

    std::cout << "Iniciando simulacion...\n";
    std::cout << "Particulas iniciales: " << contarParticulasActivas() << "\n";
    std::cout << "Obstaculos: " << obstaculos.size() << "\n";
    std::cout << "dt = " << dt << "s, pasos por segundo = " << pasos_por_segundo << "\n\n";

    // Guardar estado inicial
    guardarEstados(salida, tiempoTotal);

    while (tiempoTotal < duracion)
    {
        // Actualizar física
        actualizarPosiciones();
        detectarColisiones();

        // Guardar cada 10 pasos (reduce tamaño del archivo)
        if (paso % 10 == 0) {
            guardarEstados(salida, tiempoTotal);
        }

        tiempoTotal += dt;
        paso++;

        // Mostrar progreso cada segundo simulado
        if (paso % pasos_por_segundo == 0) {
            int activas = contarParticulasActivas();
            double energia_total = 0;
            for (const auto &p : particulas) {
                if (p.activa) {
                    energia_total += p.energiaCinetica();
                }
            }

            std::cout << "t=" << std::fixed << std::setprecision(1) << tiempoTotal
                      << "s | Particulas activas: " << activas
                      << " | Energia total: " << std::setprecision(2) << energia_total << " J\n";
        }
    }

    // Guardar estado final
    guardarEstados(salida, tiempoTotal);

    salida.close();

    // Estadísticas finales
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "SIMULACION COMPLETADA\n";
    std::cout << std::string(60, '=') << "\n";

    int activas = contarParticulasActivas();
    double energia_total = 0;
    double momento_total_x = 0;
    double momento_total_y = 0;

    std::cout << "\nEstado final de las particulas:\n";
    for (size_t i = 0; i < particulas.size(); i++) {
        if (particulas[i].activa) {
            double E = particulas[i].energiaCinetica();
            energia_total += E;
            momento_total_x += particulas[i].masa * particulas[i].vx;
            momento_total_y += particulas[i].masa * particulas[i].vy;

            std::cout << "  Particula " << i << ": "
                      << "masa=" << particulas[i].masa << "kg, "
                      << "radio=" << particulas[i].radio << "px, "
                      << "E=" << E << "J\n";
        }
    }

    std::cout << "\nConservacion:\n";
    std::cout << "  Particulas finales: " << activas << "/" << particulas.size() << "\n";
    std::cout << "  Energia cinetica total: " << energia_total << " J\n";
    std::cout << "  Momento lineal: (" << momento_total_x << ", " << momento_total_y << ") kg*m/s\n";
    std::cout << "\nDatos guardados en: " << archivo << "\n";
    std::cout << std::string(60, '=') << "\n";
}
