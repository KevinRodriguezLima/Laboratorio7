#ifndef DELAUNAY3D_HPP
#define DELAUNAY3D_HPP

#include <vector>
#include <opencv2/core.hpp>

struct Tetrahedro {
    cv::Point3f v1, v2, v3, v4;

    bool operator==(const Tetrahedro& other) const {
    return (v1 == other.v1 && v2 == other.v2 && v3 == other.v3 && v4 == other.v4);
}

};

struct Triangulo {
    cv::Point3f v1, v2, v3;
    bool operator<(const Triangulo& other) const {
        return std::tie(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z) <
               std::tie(other.v1.x, other.v1.y, other.v1.z, other.v2.x, other.v2.y, other.v2.z, other.v3.x, other.v3.y, other.v3.z);
    }
};


std::vector<Tetrahedro> triangulacionDelaunay3D(const std::vector<cv::Point3f>& puntos);

#endif
