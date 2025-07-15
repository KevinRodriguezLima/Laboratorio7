#ifndef VIEWER_HPP
#define VIEWER_HPP

#include "Mesh.hpp"

class Viewer {
public:
    static void visualizar(const Mesh& mesh);
    static void visualizarPuntos(const std::vector<cv::Point3f>& puntos);
};

#endif
