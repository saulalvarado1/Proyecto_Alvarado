#include "elipsepuntomedio.h"
#include <GL/glut.h>
#include <cmath>

static void plot4(int xc,int yc,int x,int y){
    glVertex2i(xc + x, yc + y);
    glVertex2i(xc - x, yc + y);
    glVertex2i(xc + x, yc - y);
    glVertex2i(xc - x, yc - y);
}

void elipsePuntoMedio(int xc, int yc, int rx, int ry) {
    if (rx <= 0 || ry <= 0) return;
    long rx2 = 1L * rx * rx;
    long ry2 = 1L * ry * ry;
    long x = 0;
    long y = ry;
    long px = 0;
    long py = 2 * rx2 * y;

    long p = ry2 - (rx2 * ry) + (rx2 / 4);
    glBegin(GL_POINTS);
    while (px < py) {
        plot4(xc,yc,(int)x,(int)y);
        x++;
        px += 2 * ry2;
        if (p < 0) {
            p += ry2 + px;
        } else {
            y--;
            py -= 2 * rx2;
            p += ry2 + px - py;
        }
    }

    p = (long)(ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2 * ry2);
    while (y >= 0) {
        plot4(xc,yc,(int)x,(int)y);
        y--;
        py -= 2 * rx2;
        if (p > 0) {
            p += rx2 - py;
        } else {
            x++;
            px += 2 * ry2;
            p += rx2 - py + px;
        }
    }
    glEnd();
}

