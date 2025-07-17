#include "Octree.hpp"
#include <iostream>

Octree::Octree(std::vector<cv::Point3f>& puntos, int profundidadMax, int capacidad)
    : profundidadMax(profundidadMax), capacidad(capacidad) {
    // Calcular bounding box y centro
    cv::Point3f min = puntos[0], max = puntos[0];
    for (auto& p : puntos) {
        min.x = std::min(min.x, p.x);
        min.y = std::min(min.y, p.y);
        min.z = std::min(min.z, p.z);
        max.x = std::max(max.x, p.x);
        max.y = std::max(max.y, p.y);
        max.z = std::max(max.z, p.z);
    }

    float tam = std::max({ max.x - min.x, max.y - min.y, max.z - min.z });
    cv::Point3f centro = (min + max) * 0.5f;

    raiz = std::make_unique<Nodo>(centro, tam);

    for (const auto& punto : puntos) {
        insertar(raiz.get(), punto, 0);
    }
}

void Octree::insertar(Nodo* nodo, const cv::Point3f& punto, int profundidad) {
    if (nodo->esHoja) {
        nodo->puntos.push_back(punto);
        if (nodo->puntos.size() > capacidad && profundidad < profundidadMax) {
            subdividirNodo(nodo, profundidad);
        }
    } else {
        int index = 0;
        if (punto.x > nodo->centro.x) index |= 1;
        if (punto.y > nodo->centro.y) index |= 2;
        if (punto.z > nodo->centro.z) index |= 4;
        insertar(nodo->hijos[index].get(), punto, profundidad + 1);
    }
}

void Octree::subdividirNodo(Nodo* nodo, int profundidad) {
    float half = nodo->tam / 4.0f;
    nodo->esHoja = false;
    for (int i = 0; i < 8; ++i) {
        cv::Point3f offset(
            (i & 1 ? 1 : -1) * half,
            (i & 2 ? 1 : -1) * half,
            (i & 4 ? 1 : -1) * half
        );
        nodo->hijos[i] = std::make_unique<Nodo>(nodo->centro + offset, nodo->tam / 2.0f);
    }

    for (const auto& p : nodo->puntos) {
        int index = 0;
        if (p.x > nodo->centro.x) index |= 1;
        if (p.y > nodo->centro.y) index |= 2;
        if (p.z > nodo->centro.z) index |= 4;
        insertar(nodo->hijos[index].get(), p, profundidad + 1);
    }

    nodo->puntos.clear();  // Liberar memoria
}

void Octree::procesarNodo(Nodo* nodo, int profundidad) {
    if (!nodo) return;
    if (nodo->esHoja) {
        std::cout << "Procesando hoja con " << nodo->puntos.size() << " puntos\n";
        // Aquí podrías llamar a triangulación local
    } else {
        for (auto& hijo : nodo->hijos) {
            procesarNodo(hijo.get(), profundidad + 1);
        }
    }
}

void Octree::procesarSubregionesEnParalelo() {
    std::vector<std::thread> hilos;

    for (auto& hijo : raiz->hijos) {
        if (hijo) {
            hilos.emplace_back([this, &hijo]() {
                procesarNodo(hijo.get(), 1);
            });
        }
    }

    for (auto& h : hilos) {
        h.join();
    }
}

void Octree::obtenerHojas(std::vector<std::shared_ptr<Octree::Nodo>>& hojas) {
    std::function<void(Nodo*)> recorrer = [&](Nodo* nodo) {
        if (!nodo) return;
        if (nodo->esHoja) {
            hojas.push_back(std::shared_ptr<Octree::Nodo>(nodo, [](Octree::Nodo*){}));  // ✅ corregido
        } else {
            for (auto& hijo : nodo->hijos) {
                recorrer(hijo.get());
            }
        }
    };
    recorrer(raiz.get());
}
