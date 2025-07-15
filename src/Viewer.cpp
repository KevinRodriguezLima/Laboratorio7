#include "Viewer.hpp"
#include <GL/freeglut.h>
#include <iostream>
#include <algorithm>

static Mesh meshGlobal;
static std::vector<cv::Point3f> puntosGlobales;
static cv::Point3f centro;

static float angleX = 0.0f, angleY = 0.0f;
static int lastX = -1, lastY = -1;
static bool mouseDown = false;

static float scaleFactor = 1.0f;

enum ModoVisualizacion { MALLA, PUNTOS };
static ModoVisualizacion modo = MALLA;

cv::Point3f calcularCentro(const std::vector<cv::Point3f>& puntos) {
    if (puntos.empty()) return { 0, 0, 0 };
    cv::Point3f suma(0, 0, 0);
    for (const auto& p : puntos) {
        suma += p;
    }
    return suma * (1.0f / puntos.size());
}

void drawMesh() {
    glColor3f(0.7f, 0.4f, 0.9f);
    glBegin(GL_TRIANGLES);
    for (const auto& tri : meshGlobal.obtenerTriangulos()) {
        glVertex3f(tri.v1.x, tri.v1.y, tri.v1.z);
        glVertex3f(tri.v2.x, tri.v2.y, tri.v2.z);
        glVertex3f(tri.v3.x, tri.v3.y, tri.v3.z);
    }
    glEnd();
}

void drawPuntos() {
    glColor3f(0.0f, 1.0f, 1.0f);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (const auto& p : puntosGlobales) {
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Aleja la cámara
    glTranslatef(0.0f, 0.0f, -5.0f);

    // Centrar y aplicar rotación/escala
    glTranslatef(centro.x, centro.y, centro.z);
    glScalef(scaleFactor, scaleFactor, scaleFactor);
    glRotatef(angleX, 1.0f, 0.0f, 0.0f);
    glRotatef(angleY, 0.0f, 1.0f, 0.0f);
    glTranslatef(-centro.x, -centro.y, -centro.z);

    if (modo == MALLA)
        drawMesh();
    else
        drawPuntos();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    float ratio = static_cast<float>(w) / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, ratio, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
}

void idle() {
    glutPostRedisplay();
}

void centrarYEscalar(std::vector<cv::Point3f>& puntos) {
    if (puntos.empty()) return;

    float minX = puntos[0].x, maxX = puntos[0].x;
    float minY = puntos[0].y, maxY = puntos[0].y;
    float minZ = puntos[0].z, maxZ = puntos[0].z;

    for (const auto& p : puntos) {
        minX = std::min(minX, p.x); maxX = std::max(maxX, p.x);
        minY = std::min(minY, p.y); maxY = std::max(maxY, p.y);
        minZ = std::min(minZ, p.z); maxZ = std::max(maxZ, p.z);
    }

    float centroX = (minX + maxX) / 2.0f;
    float centroY = (minY + maxY) / 2.0f;
    float centroZ = (minZ + maxZ) / 2.0f;

    float sizeX = maxX - minX;
    float sizeY = maxY - minY;
    float sizeZ = maxZ - minZ;
    float maxSize = std::max({sizeX, sizeY, sizeZ});
    float escala = 2.0f / maxSize;

    for (auto& p : puntos) {
        p.x = (p.x - centroX) * escala;
        p.y = (p.y - centroY) * escala;
        p.z = (p.z - centroZ) * escala;
    }
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        mouseDown = (state == GLUT_DOWN);
        lastX = x;
        lastY = y;
    } else if (button == 3 && state == GLUT_DOWN) {
        scaleFactor *= 1.1f;
        if (scaleFactor > 100.0f) scaleFactor = 100.0f;
    } else if (button == 4 && state == GLUT_DOWN) {
        scaleFactor *= 0.9f;
        if (scaleFactor < 0.01f) scaleFactor = 0.01f;
    }
}

void motion(int x, int y) {
    if (mouseDown) {
        angleX += (y - lastY) * 0.5f;
        angleY += (x - lastX) * 0.5f;
        lastX = x;
        lastY = y;
    }
}

void keyboard(unsigned char key, int, int) {
    if (key == '+') {
        scaleFactor *= 1.1f;
    } else if (key == '-') {
        scaleFactor *= 0.9f;
    } else if (key == 'r') {
        angleX = angleY = 0.0f;
        scaleFactor = 1.0f;
    }
}

void inicializarGL(const char* titulo) {
    int argc = 1;
    char* argv[1] = { (char*)"viewer" };
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow(titulo);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
}

void Viewer::visualizar(const Mesh& mesh) {
    modo = MALLA;
    meshGlobal = mesh;

    // Extraer los vértices de todos los triángulos
    std::vector<cv::Point3f> puntos;
    for (const auto& tri : mesh.obtenerTriangulos()) {
        puntos.push_back(tri.v1);
        puntos.push_back(tri.v2);
        puntos.push_back(tri.v3);
    }

    // Centrar y escalar
    centrarYEscalar(puntos);
    centro = calcularCentro(puntos);

    // Actualizar la malla con los puntos escalados
    std::vector<Triangle> triangulosEscalados;
    for (size_t i = 0; i < puntos.size(); i += 3) {
        Triangle t{ puntos[i], puntos[i + 1], puntos[i + 2] };
        triangulosEscalados.push_back(t);
    }
    meshGlobal.setTriangulos(triangulosEscalados); // Necesitas agregar esta función a Mesh

    inicializarGL("Malla 3D - Reconstrucción de Rana");
    glutMainLoop();
}


void Viewer::visualizarPuntos(const std::vector<cv::Point3f>& puntos) {
    modo = PUNTOS;
    puntosGlobales = puntos;
    centrarYEscalar(puntosGlobales);
    centro = calcularCentro(puntosGlobales);
    inicializarGL("Nube de Puntos 3D");
    glutMainLoop();
}
