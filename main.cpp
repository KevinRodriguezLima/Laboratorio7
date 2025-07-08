//*

#include "ImageLoader.hpp"
#include "PointCloudGenerator.hpp"

int main() {
    std::string folder = "../data/";
    std::cout << "Cargando máscaras desde: " << folder << std::endl;

    auto slices = cargarImagenes(folder, 1.0f);
    auto cloud = crearNubeDePuntos(slices, 1.0f, 1.0f);

    std::cout << "Total de puntos generados: " << cloud.size() << std::endl;
    return 0;
}
//*/