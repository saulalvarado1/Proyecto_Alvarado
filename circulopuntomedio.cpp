#include "circulopuntomedio.h"
#include <GL/glut.h>

static void plot8(int xc,int yc,int x,int y){
    glVertex2i(xc + x, yc + y);
    glVertex2i(xc - x, yc + y);
    glVertex2i(xc + x, yc - y);
    glVertex2i(xc - x, yc - y);
    glVertex2i(xc + y, yc + x);
    glVertex2i(xc - y, yc + x);
    glVertex2i(xc + y, yc - x);
    glVertex2i(xc - y, yc - x);
}

void circuloPuntoMedio(int xc, int yc, int r) {
    if (r <= 0) return;
    int x = 0;
    int y = r;
    int d = 1 - r;
    glBegin(GL_POINTS);
    plot8(xc,yc,x,y);
    while (x < y) {
        x++;
        if (d < 0) {
            d += 2*x + 1;
        } else {
            y--;
            d += 2*(x - y) + 1;
        }
        plot8(xc,yc,x,y);
    }
    glEnd();
}

