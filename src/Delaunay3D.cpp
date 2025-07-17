#include "Delaunay3D.hpp"
#include "Octree.hpp"

#include <set>
#include <map>
#include <algorithm>
#include <cmath>
#include <future>
#include <opencv2/core.hpp>

const float EPSILON = 1e-6f;

// ========== FUNCIONES AUXILIARES ==========

bool orientacionPositiva(const cv::Point3f& a, const cv::Point3f& b,
                         const cv::Point3f& c, const cv::Point3f& d) {
    cv::Matx<double, 4, 4> A = {
        a.x, a.y, a.z, 1.0,
        b.x, b.y, b.z, 1.0,
        c.x, c.y, c.z, 1.0,
        d.x, d.y, d.z, 1.0
    };
    return cv::determinant(A) > 0;
}

bool enCircunsfera(const Tetrahedro& t, const cv::Point3f& p) {
    auto sq = [](float x) { return x * x; };

    auto det5 = [sq](const cv::Point3f& a, const cv::Point3f& b,
                     const cv::Point3f& c, const cv::Point3f& d,
                     const cv::Point3f& p) {
        cv::Matx<double, 5, 5> A = {
            a.x, a.y, a.z, sq(a.x) + sq(a.y) + sq(a.z), 1.0,
            b.x, b.y, b.z, sq(b.x) + sq(b.y) + sq(b.z), 1.0,
            c.x, c.y, c.z, sq(c.x) + sq(c.y) + sq(c.z), 1.0,
            d.x, d.y, d.z, sq(d.x) + sq(d.y) + sq(d.z), 1.0,
            p.x, p.y, p.z, sq(p.x) + sq(p.y) + sq(p.z), 1.0
        };
        return cv::determinant(A);
    };

    bool orient = orientacionPositiva(t.v1, t.v2, t.v3, t.v4);
    double det = det5(t.v1, t.v2, t.v3, t.v4, p);
    return orient ? (det > EPSILON) : (det < -EPSILON);
}

// ========== FUNCION BASE (LOCAL) ==========

std::vector<Tetrahedro> triangulacionDelaunay3DSimple(const std::vector<cv::Point3f>& puntos) {
    std::vector<Tetrahedro> tetraedros;
    if (puntos.size() < 4) return tetraedros;

    float maxCoord = 0.0f;
    for (const auto& p : puntos)
        maxCoord = std::max({maxCoord, std::abs(p.x), std::abs(p.y), std::abs(p.z)});
    float offset = maxCoord * 10.0f;

    cv::Point3f p1(-offset, -offset, -offset);
    cv::Point3f p2( offset, -offset, -offset);
    cv::Point3f p3( 0.0f,   offset, -offset);
    cv::Point3f p4( 0.0f,   0.0f,   offset);

    tetraedros.push_back({p1, p2, p3, p4});

    for (const auto& punto : puntos) {
        std::vector<Tetrahedro> aEliminar;
        std::map<Cara, int> carasContadas;

        for (const auto& t : tetraedros) {
            if (enCircunsfera(t, punto)) {
                aEliminar.push_back(t);
                carasContadas[{t.v1, t.v2, t.v3}]++;
                carasContadas[{t.v1, t.v2, t.v4}]++;
                carasContadas[{t.v1, t.v3, t.v4}]++;
                carasContadas[{t.v2, t.v3, t.v4}]++;
            }
        }

        for (const auto& t : aEliminar) {
            tetraedros.erase(std::remove(tetraedros.begin(), tetraedros.end(), t), tetraedros.end());
        }

        for (const auto& [cara, count] : carasContadas) {
            if (count == 1) {
                tetraedros.push_back({cara.a, cara.b, cara.c, punto});
            }
        }
    }

    auto esSuperVertice = [&](const cv::Point3f& v) {
        return v == p1 || v == p2 || v == p3 || v == p4;
    };

    tetraedros.erase(std::remove_if(tetraedros.begin(), tetraedros.end(),
        [&](const Tetrahedro& t) {
            return esSuperVertice(t.v1) || esSuperVertice(t.v2) ||
                   esSuperVertice(t.v3) || esSuperVertice(t.v4);
        }), tetraedros.end());

    return tetraedros;
}

// ========== FUNCION PRINCIPAL CON OCTREE + PARALELIZACION ==========

std::vector<Tetrahedro> triangulacionDelaunay3D(const std::vector<cv::Point3f>& puntos) {
    if (puntos.size() < 4) return {};

    // Calcular bounding box (aunque Octree lo puede manejar internamente, esto queda si lo necesitas externamente)
    cv::Point3f minP(FLT_MAX, FLT_MAX, FLT_MAX), maxP(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    for (const auto& p : puntos) {
        minP.x = std::min(minP.x, p.x);
        minP.y = std::min(minP.y, p.y);
        minP.z = std::min(minP.z, p.z);
        maxP.x = std::max(maxP.x, p.x);
        maxP.y = std::max(maxP.y, p.y);
        maxP.z = std::max(maxP.z, p.z);
    }

    //  Usamos Octree en lugar de OctreeNode
    Octree octree(const_cast<std::vector<cv::Point3f>&>(puntos), 6, 50);  // profundidad, capacidad

    // Obtenemos hojas del Octree (cada hoja contiene un conjunto de puntos)
    std::vector<std::shared_ptr<Octree::Nodo>> hojas;
    octree.obtenerHojas(hojas);

    // Ejecutamos triangulación en paralelo para cada hoja
    std::vector<std::future<std::vector<Tetrahedro>>> tareas;
    for (const auto& hoja : hojas) {
        tareas.push_back(std::async(std::launch::async, [hoja]() {
            return triangulacionDelaunay3DSimple(hoja->puntos);  // hoja->puntos debe estar definido en Nodo
        }));
    }

    // Unimos todos los tetraedros resultantes
    std::vector<Tetrahedro> todos;
    for (auto& t : tareas) {
        auto parcial = t.get();
        todos.insert(todos.end(), parcial.begin(), parcial.end());
    }

    return todos;
}