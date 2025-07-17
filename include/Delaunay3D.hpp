#ifndef DELAUNAY3D_HPP
#define DELAUNAY3D_HPP

#include <vector>
#include <opencv2/core.hpp>
#include <tuple>

// Comparador para cv::Point3f
struct ComparadorPuntos3D {
    bool operator()(const cv::Point3f& a, const cv::Point3f& b) const {
        return std::tie(a.x, a.y, a.z) < std::tie(b.x, b.y, b.z);
    }
};

// Comparador para vectores de puntos
struct ComparadorVectorPuntos {
    bool operator()(const std::vector<cv::Point3f>& a, const std::vector<cv::Point3f>& b) const {
        size_t n = std::min(a.size(), b.size());
        for (size_t i = 0; i < n; ++i) {
            if (std::tie(a[i].x, a[i].y, a[i].z) < std::tie(b[i].x, b[i].y, b[i].z)) return true;
            if (std::tie(b[i].x, b[i].y, b[i].z) < std::tie(a[i].x, a[i].y, a[i].z)) return false;
        }
        return a.size() < b.size();
    }
};

struct Cara {
    cv::Point3f a, b, c;

    bool operator<(const Cara& other) const {
        auto ordenar = [](const cv::Point3f& p) {
            return std::make_tuple(p.x, p.y, p.z);
        };

        std::array<cv::Point3f, 3> aPuntos = {a, b, c};
        std::array<cv::Point3f, 3> bPuntos = {other.a, other.b, other.c};
        
        std::sort(aPuntos.begin(), aPuntos.end(), [&](const auto& p1, const auto& p2) {
            return ordenar(p1) < ordenar(p2);
        });
    
        std::sort(bPuntos.begin(), bPuntos.end(), [&](const auto& p1, const auto& p2) {
            return ordenar(p1) < ordenar(p2);
        });
    
        for (int i = 0; i < 3; ++i) {
            if (ordenar(aPuntos[i]) < ordenar(bPuntos[i])) return true;
            if (ordenar(bPuntos[i]) < ordenar(aPuntos[i])) return false;
        }
    
        return false;
    }
};



struct Tetrahedro {
    cv::Point3f v1, v2, v3, v4;

    bool operator==(const Tetrahedro& other) const {
        return (v1 == other.v1 && v2 == other.v2 && v3 == other.v3 && v4 == other.v4);
    }
};

std::vector<Tetrahedro> triangulacionDelaunay3D(const std::vector<cv::Point3f>& puntos);

#endif
