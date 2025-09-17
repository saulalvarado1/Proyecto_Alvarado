#include <GL/glut.h>
#include <vector>
#include <stack>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <iostream>
#include "lineadirecta.h"
#include "circulopuntomedio.h"
#include "elipsepuntomedio.h"

int winW = 1000, winH = 700;
int gridSpacing = 20;
bool showGrid = true, showAxes = true, showCoords = true;

enum DrawMode { MODE_LINE_DIRECT, MODE_LINE_DDA, MODE_CIRCLE_MIDPOINT, MODE_ELLIPSE_MIDPOINT };
enum ShapeType { SH_LINE, SH_CIRCLE, SH_ELLIPSE };

struct Color { unsigned char r,g,b; };
struct Shape {
    ShapeType type;
    DrawMode algo;
    Color color;
    int thickness;
    std::vector<int> params;
};

DrawMode currentMode = MODE_LINE_DDA;
Color currentColor = {0,0,0};
int currentThickness = 1;

std::vector<Shape> shapes;
std::stack<Shape> redoStack;

bool waitingSecondPoint = false;
int tempX1=0, tempY1=0;
int lastMouseX=0, lastMouseY=0;

inline int IRound(double v){ return int(std::floor(v+0.5)); }

void drawThickPoint(int x,int y,int t){
    int r = t/2;
    glBegin(GL_POINTS);
    for (int dx=-r; dx<=r; ++dx)
        for (int dy=-r; dy<=r; ++dy)
            glVertex2i(x+dx,y+dy);
    glEnd();
}

void renderGridAndAxes(){
    if (showGrid) {
        glColor3ub(220,220,220);
        glBegin(GL_LINES);
        for (int x=0; x<=winW; x+=gridSpacing) {
            glVertex2i(x, 0);
            glVertex2i(x, winH);
        }
        for (int y=0; y<=winH; y+=gridSpacing) {
            glVertex2i(0, y);
            glVertex2i(winW, y);
        }
        glEnd();
    }

    if (showAxes) {
        glColor3ub(150,150,150);
        glBegin(GL_LINES);
        glVertex2i(0, winH/2);
        glVertex2i(winW, winH/2);
        glVertex2i(winW/2, 0);
        glVertex2i(winW/2, winH);
        glEnd();
    }
}

void drawShapes(){
    for (const Shape &s : shapes) {
        glColor3ub(s.color.r, s.color.g, s.color.b);
        glPointSize(1.0f);
        if (s.type == SH_LINE) {
            int x0=s.params[0], y0=s.params[1], x1=s.params[2], y1=s.params[3];
            if (s.algo == MODE_LINE_DIRECT) lineaDirecta(x0,y0,x1,y1);
            else lineaDDA(x0,y0,x1,y1);
        } else if (s.type == SH_CIRCLE) {
            int xc=s.params[0], yc=s.params[1], r=s.params[2];
            circuloPuntoMedio(xc,yc,r);
        } else if (s.type == SH_ELLIPSE) {
            int xc=s.params[0], yc=s.params[1], rx=s.params[2], ry=s.params[3];
            elipsePuntoMedio(xc,yc,rx,ry);
        }
    }
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    renderGridAndAxes();
    drawShapes();

    if (waitingSecondPoint) {
        glColor3ub(100,100,100);
        if (currentMode == MODE_LINE_DIRECT || currentMode == MODE_LINE_DDA) {
            lineaDDA(tempX1, tempY1, lastMouseX, lastMouseY);
        } else if (currentMode == MODE_CIRCLE_MIDPOINT) {
            int dx = lastMouseX - tempX1, dy = lastMouseY - tempY1;
            int r = IRound(std::sqrt(double(dx*dx+dy*dy)));
            circuloPuntoMedio(tempX1, tempY1, r);
        } else if (currentMode == MODE_ELLIPSE_MIDPOINT) {
            int rx = std::abs(lastMouseX - tempX1), ry = std::abs(lastMouseY - tempY1);
            if (rx==0) rx=1; if (ry==0) ry=1;
            elipsePuntoMedio(tempX1, tempY1, rx, ry);
        }
    }

    if (showCoords) {
        char buff[64];
        std::snprintf(buff, sizeof(buff), "(%d,%d)", lastMouseX, lastMouseY);
        glColor3ub(0,0,0);
        glRasterPos2i(8, winH - 18);
        for (char *c = buff; *c; ++c) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }

    glutSwapBuffers();
}

void passiveMotion(int x,int y){
    lastMouseX = x;
    lastMouseY = winH - 1 - y;
    if (waitingSecondPoint || showCoords) glutPostRedisplay();
}

void mouse(int button,int state,int x,int y){
    int wx = x;
    int wy = winH - 1 - y;
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (!waitingSecondPoint) {
            tempX1 = wx; tempY1 = wy;
            waitingSecondPoint = true;
        } else {
            Shape s;
            s.color = currentColor;
            s.thickness = currentThickness;
            s.algo = currentMode;
            if (currentMode == MODE_LINE_DIRECT || currentMode == MODE_LINE_DDA) {
                s.type = SH_LINE;
                s.params = { tempX1, tempY1, wx, wy };
            } else if (currentMode == MODE_CIRCLE_MIDPOINT) {
                s.type = SH_CIRCLE;
                int dx = wx - tempX1, dy = wy - tempY1;
                int r = IRound(std::sqrt(double(dx*dx + dy*dy)));
                s.params = { tempX1, tempY1, r };
            } else {
                s.type = SH_ELLIPSE;
                int rx = std::abs(wx - tempX1), ry = std::abs(wy - tempY1);
                if (rx==0) rx=1; if (ry==0) ry=1;
                s.params = { tempX1, tempY1, rx, ry };
            }
            shapes.push_back(s);
            while (!redoStack.empty()) redoStack.pop();
            waitingSecondPoint = false;
            glutPostRedisplay();
        }
    }
}

void exportPPM(const char *filename) {
    std::vector<unsigned char> pixels(winW * winH * 3);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, winW, winH, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    FILE* f = std::fopen(filename, "wb");
    if (!f) { perror("fopen"); return; }
    std::fprintf(f, "P6\n%d %d\n255\n", winW, winH);
    for (int y = winH - 1; y >= 0; --y) {
        std::fwrite(&pixels[y * winW * 3], 1, winW * 3, f);
    }
    std::fclose(f);
    std::printf("Exported %s\n", filename);
}

void menu(int id) {
    switch(id) {
        case 1: currentMode = MODE_LINE_DIRECT; break;
        case 2: currentMode = MODE_LINE_DDA; break;
        case 3: currentMode = MODE_CIRCLE_MIDPOINT; break;
        case 4: currentMode = MODE_ELLIPSE_MIDPOINT; break;
        case 10: currentColor = {0,0,0}; break;
        case 11: currentColor = {255,0,0}; break;
        case 12: currentColor = {0,255,0}; break;
        case 13: currentColor = {0,0,255}; break;
        case 20: currentThickness = 1; break;
        case 21: currentThickness = 2; break;
        case 22: currentThickness = 3; break;
        case 23: currentThickness = 5; break;
        case 30: showGrid = !showGrid; break;
        case 31: showAxes = !showAxes; break;
        case 32: showCoords = !showCoords; break;
        case 40: shapes.clear(); while (!redoStack.empty()) redoStack.pop(); break;
        case 41: if (!shapes.empty()) { redoStack.push(shapes.back()); shapes.pop_back(); } break;
        case 42: if (!redoStack.empty()) { shapes.push_back(redoStack.top()); redoStack.pop(); } break;
        case 50: exportPPM("exported_canvas.ppm"); break;
        case 60: // Atajos
            std::cout << "Atajos:\n"
                      << "  G: Grid ON/OFF\n"
                      << "  E: Ejes ON/OFF\n"
                      << "  C: Limpiar\n"
                      << "  S: Exportar\n"
                      << "  Z: Undo\n"
                      << "  Y: Redo\n"
                      << "  ESC: Salir\n";
            break;
        case 61: // Acerca de
            std::cout << "CAD 2D - Proyecto académico\n";
            std::cout << "Implementa algoritmos de rasterización clásicos.\n";
            break;
        default: break;
    }
    glutPostRedisplay();
}

void createMenus(){
    int mDraw = glutCreateMenu(menu);
    glutAddMenuEntry("Recta (Directo)", 1);
    glutAddMenuEntry("Recta (ADD/DDA)", 2);
    glutAddMenuEntry("Círculo (Punto Medio)", 3);
    glutAddMenuEntry("Elipse (Punto Medio)", 4);

    int mColor = glutCreateMenu(menu);
    glutAddMenuEntry("Negro", 10);
    glutAddMenuEntry("Rojo", 11);
    glutAddMenuEntry("Verde", 12);
    glutAddMenuEntry("Azul", 13);

    int mTh = glutCreateMenu(menu);
    glutAddMenuEntry("1 px", 20);
    glutAddMenuEntry("2 px", 21);
    glutAddMenuEntry("3 px", 22);
    glutAddMenuEntry("5 px", 23);

    int mView = glutCreateMenu(menu);
    glutAddMenuEntry("Mostrar/Ocultar cuadrícula", 30);
    glutAddMenuEntry("Mostrar/Ocultar ejes", 31);
    glutAddMenuEntry("Coordenadas del puntero", 32);

    int mTools = glutCreateMenu(menu);
    glutAddMenuEntry("Limpiar lienzo", 40);
    glutAddMenuEntry("Deshacer (Undo)", 41);
    glutAddMenuEntry("Rehacer (Redo)", 42);
    glutAddMenuEntry("Exportar PPM", 50);

    int mHelp = glutCreateMenu(menu);
    glutAddMenuEntry("Atajos", 60);
    glutAddMenuEntry("Acerca de", 61);

    glutCreateMenu(menu);
    glutAddSubMenu("Dibujo", mDraw);
    glutAddSubMenu("Color", mColor);
    glutAddSubMenu("Grosor", mTh);
    glutAddSubMenu("Vista", mView);
    glutAddSubMenu("Herramientas", mTools);
    glutAddSubMenu("Ayuda", mHelp);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void keyboard(unsigned char key, int, int){
    switch(key){
        case 'G': case 'g': showGrid = !showGrid; break;
        case 'E': case 'e': showAxes = !showAxes; break;
        case 'C': case 'c': shapes.clear(); while(!redoStack.empty()) redoStack.pop(); break;
        case 'S': case 's': exportPPM("exported_canvas.ppm"); break;
        case 'Z': case 'z': if(!shapes.empty()){ redoStack.push(shapes.back()); shapes.pop_back(); } break;
        case 'Y': case 'y': if(!redoStack.empty()){ shapes.push_back(redoStack.top()); redoStack.pop(); } break;
        case 27: exit(0); break;
    }
    glutPostRedisplay();
}

void reshape(int w,int h){
    winW = w; winH = h;
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, winW, 0, winH);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(winW, winH);
    glutInitWindowPosition(100,100);
    glutCreateWindow("CAD 2D - Modular");

    glClearColor(1,1,1,1);
    glPointSize(1.0f);

    createMenus();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(passiveMotion);

    glutMainLoop();
    return 0;
}
