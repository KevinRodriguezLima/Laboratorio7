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

        // Mostrar contornos sobre la imagen binarizada
        cv::Mat contornoImg = cv::Mat::zeros(binary.size(), CV_8UC3);
        cv::drawContours(contornoImg, contornos, -1, cv::Scalar(0, 255, 0), 1);
        cv::imshow("Contornos", contornoImg);
        cv::waitKey(1);

        for (const auto& contorno : contornos) {
            for (const auto& punto : contorno) {
                float px = punto.x * xSpacing;
                float py = punto.y * ySpacing;
                pointCloud.emplace_back(px, py, z);
            }
        }
    }

    std::cout << "Se generaron " << pointCloud.size() << " puntos 3D a partir de contornos.\n";
    return pointCloud;
}
