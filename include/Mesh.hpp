#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <opencv2/core.hpp>
#include "Delaunay3D.hpp"

struct Triangle {
    cv::Point3f v1, v2, v3;
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

#endif
