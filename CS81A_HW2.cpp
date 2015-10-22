/**************************************/
/* CS81A Fall 2015                    */
/* HW2: Color Wheel                   */
/* Student Name: Robert Metchev       */
/* SID: 10668730                      */
/**************************************/

// main

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>
using namespace std;

//----- GLOBAL VARIABLES ------------
const double PI = 3.141592653589793;
const int N_SEGMENTS = 64;// 256; // segments in circle
const int N_RINGS = 8; // rings in circle
const int N_VERTEX = N_SEGMENTS * (1 + N_RINGS); // vertices in circle
const int VERT_BUF_SZ = 3 * N_VERTEX; // # of 3 doubles vertices
const int IDX_BUF_SZ = (2 * N_SEGMENTS + 2) * N_RINGS; // vertex index

int button, state = 1;
double gx, gy, gr, gtheta;// gx and gy are coordinates of 
                        // a clicked point on screen
double R, G, B;
double x_win = 512.0;
double y_win = 512.0;
double radiusOuter = 0.9 * 0.5;
double radiusInner = 0.3 * 0.5;

double vertPtr[VERT_BUF_SZ], colorPtr[VERT_BUF_SZ];
int vertIdx[IDX_BUF_SZ];

//----- FUNCTION DECLARATION --------
void initScreen();
void display();
void mouseClicks(int but, int sta, int x, int y);
void getGxGy(int x, int y);
void getRTheta();
bool inCircle();
void coordViaHslToRgb();

//===== main function ==================================
int main(int argc, char**argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Create a window
    glutCreateWindow("ASSIGNMENT 2: Color Wheel");
    glutPositionWindow(100, 100);
    glutReshapeWindow(x_win, y_win);

    // Program start here...
    initScreen();
    glutDisplayFunc(display);
    glutMouseFunc(mouseClicks); 
    glutMainLoop();
    return 0;
}

//===== initScreen: set up the 2D canvas ==================
void initScreen(){
    glClearColor(0.0, 0.0, 0., 0.);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 1.0, 0.0, 1.0);	// set coord for viewport
    glViewport(0, 0, x_win, y_win);	// set viewport to window

    // initialize color Point size
    glPointSize(min(x_win, y_win) * 2 * radiusOuter / 3);

    // populate arrays of verts along circles made of vertices
    gtheta = 0;
    double thetaInc = 2 * PI / N_SEGMENTS;
    for (int i = 0; i < N_SEGMENTS; i++) {
        double s = sin(gtheta), c = cos(gtheta);
        for (int r = 0; r <= N_RINGS; r++) {
            int j = N_SEGMENTS * r + i;
            gr = r * (radiusOuter - radiusInner) / N_RINGS + radiusInner;
            gx = 0.5 + gr * c; // convert polar to cartesian
            gy = 0.5 + gr * s; // convert polar to cartesian
            coordViaHslToRgb(); // get colors based on polar coordinates
            int k = 3 * j; // 3x for gl primitives
            vertPtr[k + 0] = gx; vertPtr[k + 1] = gy; vertPtr[k + 2] = 0;
            colorPtr[k + 0] = R; colorPtr[k + 1] = G; colorPtr[k + 2] = B;
        }
        gtheta += thetaInc;
    }

    // populate arrays of indices to buils triangle strips
    int m = 0;
    for (int r = 0; r <= N_RINGS; r++) {
        for (int i = 0; i < N_SEGMENTS; i++) {
            vertIdx[m++] = i + N_SEGMENTS * r;
            vertIdx[m++] = i + N_SEGMENTS * (r + 1);
        }
        vertIdx[m++] = N_SEGMENTS * r ;
        vertIdx[m++] = N_SEGMENTS * (r + 1);
    }
}

//===== display: perform OpenGL drawing on the canvas =========
void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // draw Circles from array
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_DOUBLE, 0, vertPtr);
    glColorPointer(3, GL_DOUBLE, 0, colorPtr);

    for (int i = 0; i < N_RINGS; i++)
        glDrawElements(GL_TRIANGLE_STRIP, 2 * N_SEGMENTS + 2, GL_UNSIGNED_INT, vertIdx + i * (2 * N_SEGMENTS + 2));

    //glDisableClientState(GL_VERTEX_ARRAY);
    //glDisableClientState(GL_COLOR_ARRAY);

    // draw selected color square - if selected
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && inCircle()) {
        glBegin(GL_POINTS);
        glColor3f(R, G, B);
        glVertex3f(0.5, 0.5, 1.0);
        glEnd();
    }
    glutSwapBuffers(); // also flushes
}

//===== mouseClicks: handle left mouse click actions =========
void mouseClicks(int but, int sta, int x, int y) {
    button = but;
    state = sta;
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        getGxGy(x, y);
        getRTheta();
        coordViaHslToRgb();
    }
    glutPostRedisplay(); // tell glutMainLoop to execute glutDisplayFunc
}

void getGxGy(int x, int y) {
    gx = double(x) / x_win;
    gy = double(y_win - y) / y_win;
}

void getRTheta() {
    gr = sqrt(pow(gx - 0.5, 2) + pow(gy - 0.5, 2));
    gtheta = atan2((gy - 0.5), (gx - 0.5));
    if (gtheta < 0) gtheta += 2 * PI;
}
// we don't want to draw outside of the circle
bool inCircle() {
    return gr <= radiusOuter;
}

// convert polar to RGB using HSL equations
void coordViaHslToRgb() {
    double t = 6 * gtheta / (2 * PI);
         if (t < 1) { R = 1; G = t - 0;  B = 0; }
    else if (t < 2) { R = 2 - t; G = 1;  B = 0; }
    else if (t < 3) { R = 0; G = 1;  B = t - 2; }
    else if (t < 4) { R = 0; G = 4 - t;  B = 1; }
    else if (t < 5) { R = t - 4; G = 0;  B = 1; }
    else if (t < 6) { R = 1; G = 0;  B = 6 - t; }

    double rCutoff = (radiusOuter + radiusInner) * 0.5;
    if (gr < rCutoff) {
        R = (gr - radiusInner) * R / (rCutoff - radiusInner);
        G = (gr - radiusInner) * G / (rCutoff - radiusInner);
        B = (gr - radiusInner) * B / (rCutoff - radiusInner);
    }
    else {
        R = R + (1 - R) * (gr - rCutoff) / (radiusOuter - rCutoff);
        G = G + (1 - G) * (gr - rCutoff) / (radiusOuter - rCutoff);
        B = B + (1 - B) * (gr - rCutoff) / (radiusOuter - rCutoff);
    }
}
