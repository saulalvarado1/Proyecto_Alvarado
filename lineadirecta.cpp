#include "lineadirecta.h"
#include <GL/glut.h>
#include <cmath>
#include <algorithm>

void lineaDirecta(int x1, int y1, int x2, int y2) {
    glBegin(GL_POINTS);
    if (x1 == x2) {
        int ymin = std::min(y1,y2), ymax = std::max(y1,y2);
        for (int y=ymin; y<=ymax; ++y) glVertex2i(x1,y);
    } else if (y1 == y2) {
        int xmin = std::min(x1,x2), xmax = std::max(x1,x2);
        for (int x=xmin; x<=xmax; ++x) glVertex2i(x,y1);
    } else {
        float m = float(y2 - y1) / float(x2 - x1);
        float b = y1 - m * x1;
        if (std::fabs(m) <= 1.0f) {
            int xmin = std::min(x1,x2), xmax = std::max(x1,x2);
            for (int x = xmin; x <= xmax; ++x) {
                int y = int(std::floor(m * x + b + 0.5f));
                glVertex2i(x, y);
            }
        } else {
            int ymin = std::min(y1,y2), ymax = std::max(y1,y2);
            for (int y = ymin; y <= ymax; ++y) {
                int x = int(std::floor((y - b) / m + 0.5f));
                glVertex2i(x, y);
            }
        }
    }
    glEnd();
}

void lineaDDA(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = std::max(std::abs(dx), std::abs(dy));
    if (steps == 0) {
        glBegin(GL_POINTS); glVertex2i(x1,y1); glEnd(); return;
    }
    float xInc = dx / float(steps);
    float yInc = dy / float(steps);
    float x = x1, y = y1;
    glBegin(GL_POINTS);
    for (int i=0;i<=steps;i++) {
        glVertex2i(int(std::floor(x+0.5f)), int(std::floor(y+0.5f)));
        x += xInc; y += yInc;
    }
    glEnd();
}

