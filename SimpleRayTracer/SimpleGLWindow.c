	/* sets up flat mesh */
/* light source position rotates in time */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define N 256
#define a 0.1
#define b 50.0
#define c 0.1

const GLdouble nearVal     = 1.0;
const GLdouble farVal      = 20.0;


static void init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.75,0.75,-0.75,0.75,-5.5,5.5);

    glEnable(GL_DEPTH_TEST);
}

static void draw(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
    // Draw a triangle
    glPushMatrix();
    glScaled(0.25, 0.25, 0.25);

    glBegin(GL_TRIANGLES);
    glColor3f(0.0, 0.0, 1.0);  /* blue */
    glVertex2d(-1, -1);
    glColor3f(0.0, 1.0, 0.0);  /* green */
    glVertex2d(0, 1);
    glColor3f(1.0, 0.0, 0.0);  /* red */
    glVertex2d(1, -1);
    glEnd();
    glPopMatrix();
    
    glutSwapBuffers();
}

static void reshape(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.75,0.75,-0.75,0.75,-5.5,5.5);

    glViewport(0, 0, w, h);
    glutPostRedisplay();
}

static void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 27:
    case 'Q':
    case 'q':
        exit(EXIT_SUCCESS);
        break;
    default:
        break;
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    #ifndef __APPLE__
        glewInit();
    #endif
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1024, 1024);
    glutCreateWindow("Simple GLUT example");
    glutDisplayFunc(draw);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    init();
   

    glutMainLoop();
    return 0;
}
