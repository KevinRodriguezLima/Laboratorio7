#include "PointCloudGenerator.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>

std::vector<cv::Point3f> crearNubeDePuntos(const std::vector<MaskSlice>& slices, float xSpacing, float ySpacing) {
    std::vector<cv::Point3f> pointCloud;

    for (const auto& slice : slices) {
        const cv::Mat& binary = slice.binaryImage;
        float z = slice.z;

        // Mostrar
        cv::imshow("Slice binarizada", binary);
        cv::waitKey(1); 

        for (int y = 0; y < binary.rows; ++y) {
            for (int x = 0; x < binary.cols; ++x) {
                if (binary.at<uchar>(y, x) > 0) {
                    float px = x * xSpacing;
                    float py = y * ySpacing;
                    pointCloud.emplace_back(px, py, z);
                }
            }
        }
    }

    std::cout << "Se generaron " << pointCloud.size() << " puntos 3D.\n";
    return pointCloud;
}
