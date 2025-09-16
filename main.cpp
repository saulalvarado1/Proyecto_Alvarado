#include <GL/glut.h>
#include <cstdlib>

int winW = 800, winH = 600;

bool showGrid = true;
bool showAxes = true;
int gridSpacing = 20;

void drawGrid() {
    glColor3ub(220, 220, 220);
    glBegin(GL_POINTS);
    for (int x = 0; x <= winW; x += gridSpacing) {
        for (int y = 0; y <= winH; y += gridSpacing) {
            glVertex2i(x, y);
        }
    }
    glEnd();
}

void drawAxes() {
    glColor3ub(150, 0, 0);
    glBegin(GL_POINTS);
    for (int x = 0; x <= winW; x++) glVertex2i(x, winH / 2);
    for (int y = 0; y <= winH; y++) glVertex2i(winW / 2, y);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (showGrid) drawGrid();
    if (showAxes) drawAxes();

    glColor3ub(255, 0, 0);
    glBegin(GL_POINTS);
        glVertex2i(winW / 2, winH / 2);
    glEnd();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 'G': case 'g': showGrid = !showGrid; break;
        case 'E': case 'e': showAxes = !showAxes; break;
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
    glutCreateWindow("Parte 2 - Grid y Ejes");

    glClearColor(1, 1, 1, 1);
    glPointSize(2.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
