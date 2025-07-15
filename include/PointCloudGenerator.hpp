#ifndef POINT_CLOUD_GENERATOR_HPP
#define POINT_CLOUD_GENERATOR_HPP

#include <vector>
#include <opencv2/opencv.hpp>
#include "ImageLoader.hpp" 

// Devuelve una nube de puntos (coordenadas 3D) a partir de las máscaras
std::vector<cv::Point3f> crearNubeDePuntos(
    const std::vector<MaskSlice>& slices,
    float xSpacing = 1.0f,
    float ySpacing = 1.0f
);

#endif
