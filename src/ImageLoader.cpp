#include "ImageLoader.hpp"
#include <filesystem>
#include <algorithm>
#include <iostream>

namespace fs = std::filesystem;

// Carga una lista de MaskSlice desde una carpeta de un órgano
std::vector<MaskSlice> cargarImagenes(const std::string& organPath, float zSpacing) {
    std::vector<MaskSlice> slices;

    std::vector<fs::directory_entry> tiffFiles;
    for (const auto& entry : fs::directory_iterator(organPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".tiff") {
            tiffFiles.push_back(entry);
        }
    }

    std::sort(tiffFiles.begin(), tiffFiles.end(), [](const fs::directory_entry& a, const fs::directory_entry& b) {
        return a.path().filename().string() < b.path().filename().string();
    });

    for (const auto& fileEntry : tiffFiles) {
        const auto& path = fileEntry.path().string();
        std::vector<cv::Mat> pages;

        if (!cv::imreadmulti(path, pages, cv::IMREAD_UNCHANGED)) {
            std::cerr << "No se pudo leer como multipagina: " << path << std::endl;
            continue;
        }

        std::cout << "Archivo: " << path << " contiene " << pages.size() << " paginas.\n";

        for (size_t i = 0; i < pages.size(); ++i) {
            cv::Mat page = pages[i];

            if (page.empty()) continue;

            double minVal, maxVal;
            cv::minMaxLoc(page, &minVal, &maxVal);
            std::cout << "  Pagina " << i << " - Min: " << minVal << ", Max: " << maxVal << "\n";

            // Normalizar y convertir a 8 bits
            cv::Mat normalized;
            cv::normalize(page, normalized, 0, 255, cv::NORM_MINMAX);
            normalized.convertTo(normalized, CV_8U);

            cv::Mat binary;
            cv::threshold(normalized, binary, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

            int nonZero = cv::countNonZero(binary);
            std::cout << "  Pagina " << i << " tiene " << nonZero << " pixeles blancos\n";

            // Visualización (opcional)
            // cv::imshow("Página binarizada", binary);
            // cv::waitKey(0);

            MaskSlice slice;
            slice.binaryImage = binary;
            slice.z = static_cast<float>(slices.size()) * zSpacing;
            slices.push_back(slice);
        }
    }

    return slices;
}


std::map<std::string, std::vector<MaskSlice>> loadAllOrgans(const std::string& dataPath, float zSpacing) {
    std::map<std::string, std::vector<MaskSlice>> organMap;

    for (const auto& entry : fs::directory_iterator(dataPath)) {
        if (entry.is_directory()) {
            std::string organName = entry.path().filename().string();
            std::string organPath = entry.path().string();

            std::cout << "Cargando órgano: " << organName << std::endl;
            auto slices = cargarImagenes(organPath, zSpacing);

            if (!slices.empty()) {
                organMap[organName] = slices;
            } else {
                std::cerr << "No se cargaron máscaras para: " << organName << std::endl;
            }
        }
    }

    return organMap;
}
