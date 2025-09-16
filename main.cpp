#include <GL/glut.h>
#include <cmath>
#include <iostream>

int winW = 800, winH = 600;

bool showGrid = true;
bool showAxes = true;

void putPixel(int x, int y) {
    glBegin(GL_POINTS);
        glVertex2i(x, y);
    glEnd();
}

void drawGrid(int spacing = 20) {
    glColor3ub(200, 200, 200);
    glBegin(GL_LINES);
    for (int x = 0; x <= winW; x += spacing) {
        glVertex2i(x, 0);
        glVertex2i(x, winH);
    }
    for (int y = 0; y <= winH; y += spacing) {
        glVertex2i(0, y);
        glVertex2i(winW, y);
    }
    glEnd();
}

void drawAxes() {
    glColor3ub(0, 0, 0);
    glBegin(GL_LINES);
        glVertex2i(winW / 2, 0);
        glVertex2i(winW / 2, winH);
        glVertex2i(0, winH / 2);
        glVertex2i(winW, winH / 2);
    glEnd();
}

void drawLineDirect(int x1, int y1, int x2, int y2) {
    if (x1 == x2) {
        for (int y = std::min(y1, y2); y <= std::max(y1, y2); y++)
            putPixel(x1, y);
        return;
    }

    float m = (float)(y2 - y1) / (x2 - x1);
    float b = y1 - m * x1;

    if (fabs(m) <= 1) {
        for (int x = std::min(x1, x2); x <= std::max(x1, x2); x++) {
            int y = round(m * x + b);
            putPixel(x, y);
        }
    } else {
        for (int y = std::min(y1, y2); y <= std::max(y1, y2); y++) {
            int x = round((y - b) / m);
            putPixel(x, y);
        }
    }
}

void drawLineDDA(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;

    int steps = std::max(abs(dx), abs(dy));
    float xInc = dx / (float)steps;
    float yInc = dy / (float)steps;

    float x = x1, y = y1;
    for (int i = 0; i <= steps; i++) {
        putPixel(round(x), round(y));
        x += xInc;
        y += yInc;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (showGrid) drawGrid();
    if (showAxes) drawAxes();

    glColor3ub(255, 0, 0);
    putPixel(winW / 2, winH / 2);

    glColor3ub(0, 0, 255);
    drawLineDirect(100, 100, 300, 200);
    drawLineDirect(100, 100, 200, 300);
    drawLineDirect(400, 100, 400, 300);
    drawLineDirect(100, 400, 300, 400);

    glColor3ub(0, 150, 0);
    drawLineDDA(500, 100, 700, 200);
    drawLineDDA(500, 100, 600, 300);
    drawLineDDA(600, 400, 600, 500);
    drawLineDDA(500, 500, 700, 500);

    glutSwapBuffers();
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 'g': case 'G': showGrid = !showGrid; break;
        case 'e': case 'E': showAxes = !showAxes; break;
        case 27: exit(0);
    }
    glutPostRedisplay();
}

void reshape(int w, int h) {
    winW = w; winH = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, winW, 0, winH);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("Parte 3 - Rectas Directo y DDA");

    glClearColor(1, 1, 1, 1);
    glPointSize(3.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
