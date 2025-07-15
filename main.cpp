
#include "ImageLoader.hpp"
#include "PointCloudGenerator.hpp"
#include "Delaunay3D.hpp"
#include "Mesh.hpp"
#include "Viewer.hpp"

int main() {
    // Puedes cambiar este subdirectorio para probar otros órganos
    std::string organoFolder = "../data/";  // <- asegúrate que esta ruta exista
    float zSpacing = 1.0f;
    float xSpacing = 1.0f;
    float ySpacing = 1.0f;

    std::cout << "Cargando máscaras desde: " << organoFolder << std::endl;

    auto slices = cargarImagenes(organoFolder, zSpacing);
    if (slices.empty()) {
        std::cerr << "No se encontraron imagenes válidas en la carpeta: " << organoFolder << std::endl;
        return 1;
    }

    std::cout << "Procesando " << slices.size() << " cortes del organo..." << std::endl;

    auto cloud = crearNubeDePuntos(slices, xSpacing, ySpacing);
    std::cout << "Puntos generados: " << cloud.size() << std::endl;

    //Viewer::visualizarPuntos(cloud);

    auto tetraedros = triangulacionDelaunay3D(cloud);
    std::cout << "Tetraedros generados: " << tetraedros.size() << std::endl;

    Mesh mesh;
    mesh.generarDesdeTetraedros(tetraedros);

    std::cout << "Mostrando malla generada..." << std::endl;
    Viewer::visualizar(mesh);  // Entrará al bucle principal de FreeGLUT

    return 0;
}

