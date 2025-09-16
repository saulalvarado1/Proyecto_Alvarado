#include <GL/glut.h>

int winW = 800, winH = 600;

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3ub(255, 0, 0);
    glBegin(GL_POINTS);
        glVertex2i(winW / 2, winH / 2);
    glEnd();

    glutSwapBuffers();
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
    glutCreateWindow("Parte 1 - Ventana basica");

    glClearColor(1, 1, 1, 1);
    glPointSize(4.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
