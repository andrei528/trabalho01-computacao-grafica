#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <cmath>

using namespace std;

// ESTRUTURAS
struct Vertice {
    double x;
    double y;
    double z;
};

using lista_vertices = vector<Vertice>;
using aresta = pair<int, int>;
using lista_arestas = vector<aresta>;

// POLIGONO
struct Poligono {
    double tamanhoLado;
    int numVertices;
    Vertice posicao;
    Vertice escala;
    double rotacaoX;
    double rotacaoY;
    double rotacaoZ;
    lista_vertices vertices;
    lista_arestas arestas;
};

// PROTOTIPOS
Poligono criar_cubo(double posicao_x, double posicao_y, double posicao_z,
                    double tamanho_lado);

void desenhar(Poligono poligono);

void movimentar(Poligono& poligono,
                double distancia,
                double anguloHorizontal,
                double anguloVertical);

void escalar(Poligono& poligono,
             double escala_x,
             double escala_y,
             double escala_z);

void rotacionarX(Poligono& poligono, double angulo);
void rotacionarY(Poligono& poligono, double angulo);
void rotacionarZ(Poligono& poligono, double angulo);

Vertice obterCentro(Poligono poligono);

void display();
void redraw(int value);
void keyboard(unsigned char key, int x, int y);
void keyboard_special(int key, int x, int y);

// VARIÁVEIS GLOBAIS
Poligono cubo;

int delay = 10;

// MAIN
int main(int argc, char** argv) {

    // Cria um cubo de lado 60
    // centralizado em (0, 0, 0)
    cubo = criar_cubo(0, 0, 0, 60);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Cubo 3D - Computacao Grafica");

    // Cor do fundo
    glClearColor(1.0, 1.0, 1.0, 1.0);

    // Ativa o teste de profundidade
    glEnable(GL_DEPTH_TEST);

    // PROJEÇÃO EM PERSPECTIVA
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // esquerda, direita, baixo, cima, perto, longe
    glFrustum(-1.0, 1.0,
              -0.75, 0.75,
              1.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);

    // Eventos
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboard_special);
    glutTimerFunc(delay, redraw, 0);
    glutMainLoop();

    return 0;
}

// DISPLAY
void display(void) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    // CÂMERA
    //
    // O cubo está próximo da origem.
    // A câmera é colocada em z = 300.
    //
    // Como a câmera padrão olha para -Z,
    // fazemos a câmera "recuar" 300 unidades.
    //

    glTranslated(0, 0, -300);


    desenhar(cubo);


    glFlush();
}

// CRIAÇÃO DO CUBO
Poligono criar_cubo(double posicao_x,
                    double posicao_y,
                    double posicao_z,
                    double tamanho_lado) {

    Poligono novo_cubo;

    novo_cubo.tamanhoLado = tamanho_lado;

    novo_cubo.numVertices = 8;

    novo_cubo.posicao.x = posicao_x;
    novo_cubo.posicao.y = posicao_y;
    novo_cubo.posicao.z = posicao_z;

    novo_cubo.escala.x = 1;
    novo_cubo.escala.y = 1;
    novo_cubo.escala.z = 1;

    novo_cubo.rotacaoX = 0;
    novo_cubo.rotacaoY = 0;
    novo_cubo.rotacaoZ = 0;

    double metade = tamanho_lado / 2.0;

    // VÉRTICES DO CUBO
    novo_cubo.vertices.push_back({ -metade, -metade,  metade }); // 0
    novo_cubo.vertices.push_back({  metade, -metade,  metade }); // 1
    novo_cubo.vertices.push_back({  metade, -metade, -metade }); // 2
    novo_cubo.vertices.push_back({ -metade, -metade, -metade }); // 3

    novo_cubo.vertices.push_back({ -metade,  metade,  metade }); // 4
    novo_cubo.vertices.push_back({  metade,  metade,  metade }); // 5
    novo_cubo.vertices.push_back({  metade,  metade, -metade }); // 6
    novo_cubo.vertices.push_back({ -metade,  metade, -metade }); // 7

    // ARESTAS DO CUBO
    // Face frontal
    novo_cubo.arestas.push_back({ 0, 1 });
    novo_cubo.arestas.push_back({ 1, 2 });
    novo_cubo.arestas.push_back({ 2, 3 });
    novo_cubo.arestas.push_back({ 3, 0 });

    // Face traseira
    novo_cubo.arestas.push_back({ 4, 5 });
    novo_cubo.arestas.push_back({ 5, 6 });
    novo_cubo.arestas.push_back({ 6, 7 });
    novo_cubo.arestas.push_back({ 7, 4 });

    // Arestas conectando as duas faces
    novo_cubo.arestas.push_back({ 0, 4 });
    novo_cubo.arestas.push_back({ 1, 5 });
    novo_cubo.arestas.push_back({ 2, 6 });
    novo_cubo.arestas.push_back({ 3, 7 });

    return novo_cubo;
}

// CALCULAR CENTRO
Vertice obterCentro(Poligono poligono) {

    Vertice centro = { 0, 0, 0 };

    for (int i = 0; i < poligono.vertices.size(); i++) {

        centro.x += poligono.vertices[i].x;
        centro.y += poligono.vertices[i].y;
        centro.z += poligono.vertices[i].z;
    }

    centro.x /= poligono.vertices.size();
    centro.y /= poligono.vertices.size();
    centro.z /= poligono.vertices.size();

    return centro;
}

// MOVIMENTAÇÃO / TRANSLAÇÃO
void movimentar(Poligono& poligono,
                double distancia,
                double anguloHorizontal,
                double anguloVertical) {

    double dx = distancia *
                cos(anguloHorizontal) *
                cos(anguloVertical);

    double dy = distancia *
                sin(anguloVertical);

    double dz = distancia *
                sin(anguloHorizontal) *
                cos(anguloVertical);

    for (int i = 0; i < poligono.vertices.size(); i++) {

        poligono.vertices[i].x += dx;
        poligono.vertices[i].y += dy;
        poligono.vertices[i].z += dz;
    }

    poligono.posicao.x += dx;
    poligono.posicao.y += dy;
    poligono.posicao.z += dz;
}

// ESCALA
void escalar(Poligono& poligono,
             double escala_x,
             double escala_y,
             double escala_z) {

    // Primeiro descobrimos o centro
    Vertice centro = obterCentro(poligono);

    for (int i = 0; i < poligono.vertices.size(); i++) {

        // Transladamos o vértice para o centro virar a origem
        double x = poligono.vertices[i].x - centro.x;
        double y = poligono.vertices[i].y - centro.y;
        double z = poligono.vertices[i].z - centro.z;

        // Escala
        x *= escala_x;
        y *= escala_y;
        z *= escala_z;

        // Voltamos para a posição original
        poligono.vertices[i].x = x + centro.x;
        poligono.vertices[i].y = y + centro.y;
        poligono.vertices[i].z = z + centro.z;
    }

    poligono.escala.x *= escala_x;
    poligono.escala.y *= escala_y;
    poligono.escala.z *= escala_z;
}

// ROTAÇÃO EM X
void rotacionarX(Poligono& poligono, double angulo) {

    Vertice centro = obterCentro(poligono);

    double seno = sin(angulo);
    double cosseno = cos(angulo);

    for (int i = 0; i < poligono.vertices.size(); i++) {

        // Translada para o centro
        double x = poligono.vertices[i].x - centro.x;
        double y = poligono.vertices[i].y - centro.y;
        double z = poligono.vertices[i].z - centro.z;

        // Matriz de rotação X
        double novo_y = y * cosseno - z * seno;
        double novo_z = y * seno + z * cosseno;

        // Volta para a posição original
        poligono.vertices[i].x = x + centro.x;
        poligono.vertices[i].y = novo_y + centro.y;
        poligono.vertices[i].z = novo_z + centro.z;
    }
    poligono.rotacaoX += angulo;
}

// ROTAÇÃO EM Y
void rotacionarY(Poligono& poligono, double angulo) {

    Vertice centro = obterCentro(poligono);

    double seno = sin(angulo);
    double cosseno = cos(angulo);

    for (int i = 0; i < poligono.vertices.size(); i++) {

        double x = poligono.vertices[i].x - centro.x;
        double y = poligono.vertices[i].y - centro.y;
        double z = poligono.vertices[i].z - centro.z;

        // Matriz de rotação Y
        double novo_x = x * cosseno + z * seno;
        double novo_z = -x * seno + z * cosseno;

        poligono.vertices[i].x = novo_x + centro.x;
        poligono.vertices[i].y = y + centro.y;
        poligono.vertices[i].z = novo_z + centro.z;
    }

    poligono.rotacaoY += angulo;
}

// ROTAÇÃO EM Z
void rotacionarZ(Poligono& poligono, double angulo) {

    Vertice centro = obterCentro(poligono);

    double seno = sin(angulo);
    double cosseno = cos(angulo);

    for (int i = 0; i < poligono.vertices.size(); i++) {

        double x = poligono.vertices[i].x - centro.x;
        double y = poligono.vertices[i].y - centro.y;
        double z = poligono.vertices[i].z - centro.z;

        // Matriz de rotação Z

        double novo_x = x * cosseno - y * seno;
        double novo_y = x * seno + y * cosseno;

        poligono.vertices[i].x = novo_x + centro.x;
        poligono.vertices[i].y = novo_y + centro.y;
        poligono.vertices[i].z = z + centro.z;
    }

    poligono.rotacaoZ += angulo;
}

// DESENHAR
void desenhar(Poligono poligono) {

    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINES);

    for (int i = 0; i < poligono.arestas.size(); i++) {

        int v_o = poligono.arestas[i].first;
        int v_d = poligono.arestas[i].second;

        glVertex3d(
            poligono.vertices[v_o].x,
            poligono.vertices[v_o].y,
            poligono.vertices[v_o].z
        );


        glVertex3d(
            poligono.vertices[v_d].x,
            poligono.vertices[v_d].y,
            poligono.vertices[v_d].z
        );
    }
    glEnd();
}

// TECLADO
void keyboard(unsigned char key, int x, int y) {

    const double PI = 3.14159265359;
    const double ANGULO = 10.0 * PI / 180.0;

    switch (key) {

    // ESC
    case 27:
        exit(0);
        break;
		
    // ESCALA PARA CIMA
    case '+':

    case '=':
        escalar(cubo, 1.1, 1.1, 1.1);
        break;

    // ESCALA PARA BAIXO
    case '-':
        escalar(cubo, 0.9, 0.9, 0.9);
        break;

    // ROTAÇÃO X
    case 'x':
        rotacionarX(cubo, ANGULO);
        break;
    case 'X':
        rotacionarX(cubo, -ANGULO);
        break;

    // ROTAÇÃO Y
    case 'y':
        rotacionarY(cubo, ANGULO);
        break;
    case 'Y':
        rotacionarY(cubo, -ANGULO);
        break;

    // ROTAÇÃO Z
    case 'z':
        rotacionarZ(cubo, ANGULO);
        break;
    case 'Z':
        rotacionarZ(cubo, -ANGULO);
        break;

    // TRANSLADAÇÃO NO EIXO Z
    case 'w':
        movimentar(cubo, 10, PI / 2, PI / 2);
        break;
    case 's':
        movimentar(cubo, 10, PI / 2, -PI / 2);
        break;
    case 'a':
        movimentar(cubo, 10, PI, 0);
        break;
    case 'd':
        movimentar(cubo, 10, 0, 0);
        break;

    // Aproximar
    case 'q':
        movimentar(cubo, 10, PI / 2, 0);
        break;

    // Afastar
    case 'e':
        movimentar(cubo, 10, -PI / 2, 0);
        break;
    }
}

// TECLADO ESPECIAL
void keyboard_special(int key, int x, int y) {

    const double PI = 3.14159265359;


    switch (key) {

    case GLUT_KEY_UP:
        movimentar(cubo, 10, PI / 2, PI / 2);
        break;


    case GLUT_KEY_DOWN:
        movimentar(cubo, 10, PI / 2, -PI / 2);
        break;


    case GLUT_KEY_RIGHT:
        movimentar(cubo, 10, 0, 0);
        break;


    case GLUT_KEY_LEFT:
        movimentar(cubo, 10, PI, 0);
        break;
    }
}

// REDRAW
void redraw(int value) {

    glutPostRedisplay();
    glutTimerFunc(delay, redraw, 0);
}