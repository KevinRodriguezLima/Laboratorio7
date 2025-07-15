#include "Delaunay3D.hpp"
#include <set>
#include <map>
#include <cmath>
#include <iostream>

bool circunsferaContiene(const cv::Point3f& a, const cv::Point3f& b,
                         const cv::Point3f& c, const cv::Point3f& d,
                         const cv::Point3f& p) {
    cv::Vec3f ab = b - a;
    cv::Vec3f ac = c - a;
    cv::Vec3f ad = d - a;

    cv::Matx33f A(ab.dot(ab), ab.dot(ac), ab.dot(ad),
                  ac.dot(ab), ac.dot(ac), ac.dot(ad),
                  ad.dot(ab), ad.dot(ac), ad.dot(ad));

    float detA = cv::determinant(A);
    if (std::abs(detA) < 1e-6) return false;

    cv::Vec3f ap = p - a;
    cv::Matx31f rhs(ap.dot(ab), ap.dot(ac), ap.dot(ad));

    cv::Matx33f invA = A.inv();
    cv::Matx31f x = invA * rhs;

    float dist2 = ap.dot(ap);
    float rad2 = x(0)*x(0) + x(1)*x(1) + x(2)*x(2);

    return dist2 < rad2;
}

std::vector<Tetrahedro> delaunay3D(const std::vector<cv::Point3f>& puntos) {
    std::vector<Tetrahedro> tetraedros;

    float maxCoord = 1000;
    cv::Point3f p1(-maxCoord, -maxCoord, -maxCoord);
    cv::Point3f p2( maxCoord, -maxCoord, -maxCoord);
    cv::Point3f p3( 0,        maxCoord, -maxCoord);
    cv::Point3f p4( 0,        0,         maxCoord);

    tetraedros.push_back({p1, p2, p3, p4});

    for (const auto& punto : puntos) {
        std::vector<Tetrahedro> malos;
        for (const auto& t : tetraedros) {
            if (circunsferaContiene(t.v1, t.v2, t.v3, t.v4, punto)) {
                malos.push_back(t);
            }
        }

        std::vector<Triangulo> frontera;
        for (const auto& malo : malos) {
            frontera.push_back({malo.v1, malo.v2, malo.v3});
            frontera.push_back({malo.v1, malo.v2, malo.v4});
            frontera.push_back({malo.v1, malo.v3, malo.v4});
            frontera.push_back({malo.v2, malo.v3, malo.v4});
        }

        // Elimina los tetraedros "malos"
        for (const auto& m : malos) {
            tetraedros.erase(std::remove(tetraedros.begin(), tetraedros.end(), m), tetraedros.end());
        }

        // Elimina caras duplicadas de la frontera
        std::map<Triangulo, int> conteo;
        for (const auto& f : frontera) {
            conteo[f]++;
        }

        std::vector<Triangulo> fronteraUnica;
        for (const auto& par : conteo) {
            if (par.second == 1) {
                fronteraUnica.push_back(par.first);
            }
        }

        for (const auto& cara : fronteraUnica) {
            tetraedros.push_back({cara.v1, cara.v2, cara.v3, punto});
        }
    }

    // Elimina los tetraedros que contienen vértices del supertetraedro
    tetraedros.erase(std::remove_if(tetraedros.begin(), tetraedros.end(), [&](const Tetrahedro& t) {
        return t.v1 == p1 || t.v1 == p2 || t.v1 == p3 || t.v1 == p4 ||
               t.v2 == p1 || t.v2 == p2 || t.v2 == p3 || t.v2 == p4 ||
               t.v3 == p1 || t.v3 == p2 || t.v3 == p3 || t.v3 == p4 ||
               t.v4 == p1 || t.v4 == p2 || t.v4 == p3 || t.v4 == p4;
    }), tetraedros.end());

    return tetraedros;
}
