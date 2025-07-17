#ifndef OCTREE_HPP
#define OCTREE_HPP

#include <vector>
#include <opencv2/core.hpp>
#include <array>
#include <memory>
#include <thread>
#include <mutex>

class Octree {
public:
    struct Nodo {
        cv::Point3f centro;
        float tam;
        std::vector<cv::Point3f> puntos;
        std::array<std::unique_ptr<Nodo>, 8> hijos;
        bool esHoja = true;

        Nodo(cv::Point3f c, float t) : centro(c), tam(t) {}
    };

    Octree(std::vector<cv::Point3f>& puntos, int profundidadMax = 5, int capacidad = 10);
    void subdividir();
    void procesarSubregionesEnParalelo();
    void obtenerHojas(std::vector<std::shared_ptr<Octree::Nodo>>& hojas);  // ✅
    
private:
    std::unique_ptr<Nodo> raiz;
    int profundidadMax;
    int capacidad;

    void insertar(Nodo* nodo, const cv::Point3f& punto, int profundidad);
    void subdividirNodo(Nodo* nodo, int profundidad);
    void procesarNodo(Nodo* nodo, int profundidad);
};

#endif
