#ifndef IMAGE_LOADER_HPP
#define IMAGE_LOADER_HPP

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <map>

struct MaskSlice {
    cv::Mat binaryImage;
    float z;
};

// Esta línea faltaba 👇
std::vector<MaskSlice> cargarImagenes(const std::string& organPath, float zSpacing);

std::map<std::string, std::vector<MaskSlice>> loadAllOrgans(const std::string& dataPath, float zSpacing = 1.0f);

#endif
