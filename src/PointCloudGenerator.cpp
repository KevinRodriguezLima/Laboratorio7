#include "PointCloudGenerator.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>

std::vector<cv::Point3f> crearNubeDePuntos(const std::vector<MaskSlice>& slices, float xSpacing, float ySpacing) {
    std::vector<cv::Point3f> pointCloud;

    for (const auto& slice : slices) {
        const cv::Mat& binary = slice.binaryImage;
        float z = slice.z;

        std::vector<std::vector<cv::Point>> contornos;
        cv::findContours(binary, contornos, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

        // Visualizar los contornos en una imagen en blanco
        cv::Mat contornoDisplay = cv::Mat::zeros(binary.size(), CV_8UC3);
        cv::drawContours(contornoDisplay, contornos, -1, cv::Scalar(0, 255, 0), 1);

        cv::imshow("Contorno del Slice", contornoDisplay);
        cv::waitKey(1); 

        // Generar puntos 3D a partir de los contornos
        for (const auto& contorno : contornos) {
            for (const auto& punto : contorno) {
                float px = punto.x * xSpacing;
                float py = punto.y * ySpacing;
                pointCloud.emplace_back(px, py, z);
            }
        }
    }

    std::cout << "Se generaron " << pointCloud.size() << " puntos de borde 3D.\n";

    // 🔧 Escalamiento uniforme para conservar proporciones
    if (!pointCloud.empty()) {
        cv::Point3f min = pointCloud[0];
        cv::Point3f max = pointCloud[0];

        for (const auto& p : pointCloud) {
            min.x = std::min(min.x, p.x); max.x = std::max(max.x, p.x);
            min.y = std::min(min.y, p.y); max.y = std::max(max.y, p.y);
            min.z = std::min(min.z, p.z); max.z = std::max(max.z, p.z);
        }

        float rangeX = max.x - min.x;
        float rangeY = max.y - min.y;
        float rangeZ = max.z - min.z;

        float maxRange = std::max({rangeX, rangeY, rangeZ});
        if (maxRange == 0.0f) maxRange = 1.0f;

        const float expansionFactor = 100.0f;

        for (auto& p : pointCloud) {
            p = (p - min) * (expansionFactor / maxRange);
            //std::cout << "Punto borde escalado: (" << p.x << ", " << p.y << ", " << p.z << ")\n";
        }
        std::cout << "Nube de bordes escalada y expandida x" << expansionFactor << ".\n";
    }

    return pointCloud;
}
