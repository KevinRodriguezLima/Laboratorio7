#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <opencv2/core.hpp>
#include "Delaunay3D.hpp"

struct Triangle {
    cv::Point3f a, b, c;
    Triangle() = default;
    Triangle(const cv::Point3f& a, const cv::Point3f& b, const cv::Point3f& c)
        : a(a), b(b), c(c) {}

};

class Mesh {
public:
    void generarDesdeTetraedros(const std::vector<Tetrahedro>& tetraedros);
    const std::vector<Triangle>& obtenerTriangulos() const;
    void setTriangulos(const std::vector<Triangle>& tris) {
        triangulos = tris;
    }
private:
    std::vector<Triangle> triangulos;
    

};


struct TriangleKey {
    cv::Point3f a, b, c;

    TriangleKey(const cv::Point3f& p1, const cv::Point3f& p2, const cv::Point3f& p3) {
        std::array<cv::Point3f, 3> puntos = {p1, p2, p3};
        std::sort(puntos.begin(), puntos.end(), [](const cv::Point3f& lhs, const cv::Point3f& rhs) {
            return std::tie(lhs.x, lhs.y, lhs.z) < std::tie(rhs.x, rhs.y, rhs.z);
        });
        a = puntos[0];
        b = puntos[1];
        c = puntos[2];
    }

    bool operator==(const TriangleKey& other) const {
        return a == other.a && b == other.b && c == other.c;
    }
};


struct TriangleKeyHash {
    std::size_t operator()(const TriangleKey& key) const {
        auto hashPoint = [](const cv::Point3f& p) {
            std::size_t h1 = std::hash<float>()(p.x);
            std::size_t h2 = std::hash<float>()(p.y);
            std::size_t h3 = std::hash<float>()(p.z);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        };
        return hashPoint(key.a) ^ (hashPoint(key.b) << 1) ^ (hashPoint(key.c) << 2);
    }
};


#endif
