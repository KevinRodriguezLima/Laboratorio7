#include "Mesh.hpp"
#include <unordered_map>
#include <vector>
#include <tuple>
#include <algorithm>
#include <iostream>

// Estructura para identificar triángulos únicos, sin importar el orden
struct TriangleKey {
    cv::Point3f a, b, c;

    TriangleKey(const cv::Point3f& p1, const cv::Point3f& p2, const cv::Point3f& p3) {
        std::vector<cv::Point3f> puntos = {p1, p2, p3};
        std::sort(puntos.begin(), puntos.end(), [](const cv::Point3f& p1, const cv::Point3f& p2) {
            if (p1.x != p2.x) return p1.x < p2.x;
            if (p1.y != p2.y) return p1.y < p2.y;
            return p1.z < p2.z;
        });
        a = puntos[0];
        b = puntos[1];
        c = puntos[2];
    }

    bool operator==(const TriangleKey& other) const {
        return a == other.a && b == other.b && c == other.c;
    }
};

// Hash para TriangleKey
namespace std {
    template <>
    struct hash<TriangleKey> {
        size_t operator()(const TriangleKey& tri) const {
            auto h1 = hash<float>()(tri.a.x) ^ hash<float>()(tri.a.y) ^ hash<float>()(tri.a.z);
            auto h2 = hash<float>()(tri.b.x) ^ hash<float>()(tri.b.y) ^ hash<float>()(tri.b.z);
            auto h3 = hash<float>()(tri.c.x) ^ hash<float>()(tri.c.y) ^ hash<float>()(tri.c.z);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}

void Mesh::generarDesdeTetraedros(const std::vector<Tetrahedro>& tetraedros) {
    triangulos.clear();
    std::unordered_map<TriangleKey, int> contadorCaras;

    for (const auto& t : tetraedros) {
        std::vector<std::tuple<cv::Point3f, cv::Point3f, cv::Point3f>> caras = {
            {t.v1, t.v2, t.v3},
            {t.v1, t.v2, t.v4},
            {t.v1, t.v3, t.v4},
            {t.v2, t.v3, t.v4}
        };

        for (const auto& [a, b, c] : caras) {
            TriangleKey key(a, b, c);
            contadorCaras[key]++;
        }
    }

    for (const auto& [key, count] : contadorCaras) {
        if (count == 1) {
            triangulos.push_back({key.a, key.b, key.c});
        }
    }

    std::cout << "Triángulos visibles en malla: " << triangulos.size() << std::endl;
}

const std::vector<Triangle>& Mesh::obtenerTriangulos() const {
    return triangulos;
}
