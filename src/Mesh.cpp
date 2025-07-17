    #include "Mesh.hpp"
    #include <unordered_map>
    #include <array>
    #include <vector>
    #include <tuple>
    #include <algorithm>
    #include <iostream>


    void Mesh::generarDesdeTetraedros(const std::vector<Tetrahedro>& tetraedros) {
        triangulos.clear();
        std::unordered_map<TriangleKey, int, TriangleKeyHash> contadorCaras;

        // Primero verifica si hay tetraedros para procesar
        if (tetraedros.empty()) {
            std::cout << "Advertencia: Vector de tetraedros vacío." << std::endl;
            return;
        }

        for (const auto& t : tetraedros) {
            // Las 4 caras de un tetraedro (asegúrate que Tetrahedro use v0,v1,v2,v3)
            std::vector<std::tuple<cv::Point3f, cv::Point3f, cv::Point3f>> caras = {
                {t.v1, t.v2, t.v3},  // Cara base
                {t.v1, t.v2, t.v4},  // Cara lateral 1
                {t.v1, t.v3, t.v4},  // Cara lateral 2
                {t.v2, t.v3, t.v4}   // Cara superior
            };

            for (const auto& cara : caras) {
                const auto& [a, b, c] = cara;
                TriangleKey key(a, b, c);
                
                // Incrementar el contador para esta cara
                contadorCaras[key]++;
            }
        }

        // Reservar espacio para mejorar rendimiento
        triangulos.reserve(contadorCaras.size() / 2);

        // Filtrar solo las caras con count == 1 (caras externas)
        for (const auto& [key, count] : contadorCaras) {
            if (count == 1) {
                triangulos.emplace_back(key.a, key.b, key.c);
            }
        }

        std::cout << "Triángulos en la superficie de la malla: " << triangulos.size() << std::endl;
    }

    const std::vector<Triangle>& Mesh::obtenerTriangulos() const {
        return triangulos;
    }
