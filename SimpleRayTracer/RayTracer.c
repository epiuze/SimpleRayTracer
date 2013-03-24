//
//  RayTracer.c
//  SimpleRayTracer
//
//  Created by Emmanuel Piuze-Phaneuf on 2013-03-23.
//  Copyright (c) 2013 Emmanuel Piuze-Phaneuf. All rights reserved.
//

#include "RayTracer.h"
#include "GLUtil.h"
#include <math.h>

#define DEFAULT_STEP_SIZE 0.005
#define DEFAULT_SCALE 3
#define DEFAULT_EMISSIVITY 0.15
#define DEFAULT_DEPTH 300
#define bool int
#define false 0
#define true 1

GLuint width = 500;
GLuint height= 500;

float   angle = 0.0, axis[3], trans[3];
bool    trackingMouse = false;
bool    redrawContinue = false;
bool    trackballMove = false;

#define VOLUME_TEX_SIZE 128
int size = VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE* 4;

GLuint frameBuffer, renderBuffer;
GLuint volumeTexture, backfaceTexture, imageTexture;

GLuint passthroughProgram, shaderProgram;

// uniforms
GLuint uniform_tex;
GLuint uniform_volume_tex;
GLuint uniform_stepSize;
GLuint uniform_emissivity;
GLuint uniform_scale;
GLuint uniform_epsilon;
GLuint uniform_drawBack;
GLuint uniform_drawRays;
GLuint uniform_drawFront;
GLuint uniform_rayDepth;

//GLuint tmp1[1] = {};

void drawTestTriangle() {
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
}

void draw() {
    
    glClearColor(1, 1, 1, 1);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if(trackballMove) glRotatef(angle, axis[0], axis[1], axis[2]);

//    glTranslated(0, 0, 5);
//    glUseProgram(passthroughProgram);
    glUseProgram(shaderProgram);

//    drawTestTriangle();

    // Enable the render buffers
    enable_renderbuffers();

//    glLoadIdentity();
    
    renderBackFace();
    
    renderRayCast();
    
    disable_renderbuffers();

    renderScreen();
    
    // Done!
    glutSwapBuffers();
}

void renderScreen() {
    //
    //    // Disable the render buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Render buffer to screen
    glEnable(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, imageTexture);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    
    gluOrtho2D(0, 1, 0, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Draw raycast quad
    glDisable(GL_DEPTH_TEST);
    drawFullscreenQuad();
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

}

void renderBackFace() {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, backfaceTexture, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    
    drawQuads(1);
    
    glDisable(GL_CULL_FACE);
}

void renderRayCast() {
    //    glUniform1f(timeParam, glutGet(GLUT_ELAPSED_TIME));
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, imageTexture, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    // Assign sampler IDs
    // note: those are NOT the same as texture IDs
    // they are 0, 1, ..., n corresponding to the GLSL texture IDs
    // they are the glActiveTexture(CurrentTextureUnit + SamplerID)
    // Set the texture units
    glUniform1i(uniform_tex, 0);
    glUniform1i(uniform_volume_tex, 1);
    
    // Bind the samplers
    
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_3D, volumeTexture);
    glBindSampler(0, volumeTexture);
    
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, backfaceTexture);
    glBindSampler(2, backfaceTexture);
    
    // Default resolution:
    glUniform1f(uniform_stepSize, DEFAULT_STEP_SIZE);
    glUniform1f(uniform_scale, DEFAULT_SCALE);
    glUniform1f(uniform_epsilon, 0);
    glUniform1i(uniform_rayDepth, DEFAULT_DEPTH);
    glUniform1f(uniform_emissivity, DEFAULT_EMISSIVITY);
    
    glUniform1i(uniform_drawBack, 0);
    glUniform1i(uniform_drawFront, 0);
    glUniform1i(uniform_drawRays, 1);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    drawQuads(1.0f);
    
    glDisable(GL_CULL_FACE);
}

// create a test volume texture, here you could load your own volume
void setVolume() {
//	unsigned char data[size] = {};
    unsigned char* data = (unsigned char *)malloc(size * sizeof(unsigned char));

    float rmax = RAND_MAX;
    srand(time(NULL));
	for(int x = 0; x < VOLUME_TEX_SIZE; x++) {
        for(int y = 0; y < VOLUME_TEX_SIZE; y++) {
            for(int z = 0; z < VOLUME_TEX_SIZE; z++) {
                
                data[(x*4) + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4) + 0] = 255 * (rand() / rmax);
                data[(x*4) + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4) + 1] = 255 * (rand() / rmax);
                data[(x*4) + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4) + 2] = 255 * (rand() / rmax);
                data[(x*4) + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4) + 3] = 255 * (rand() / rmax);
            }
        }
    }
    
    
    
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &volumeTexture);
	glBindTexture(GL_TEXTURE_3D, volumeTexture);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexImage3D(GL_TEXTURE_3D, 0,GL_RGBA, VOLUME_TEX_SIZE, VOLUME_TEX_SIZE,VOLUME_TEX_SIZE,0, GL_RGBA, GL_UNSIGNED_BYTE,data);
    
    free(data);
}

void setTexture3DParam(int activeSamplerTexture) {
    
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
}


void enable_renderbuffers()
{
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, frameBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER_EXT, renderBuffer);
}


void disable_renderbuffers()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void drawFullscreenQuad() {
    glBegin(GL_QUADS);
    
    glTexCoord2f(0, 0);
    glVertex2f(0, 0);
    glTexCoord2f(1, 0);
    glVertex2f(1, 0);
    glTexCoord2f(1, 1);
    glVertex2f(1, 1);
    glTexCoord2f(0, 1);
    glVertex2f(0, 1);
    
    glEnd();
    
}

void vertex(float x, float y, float z) {
    glColor3f(x,y,z);
    glMultiTexCoord3f(GL_TEXTURE1, x, y, z);
//    glVertex3f(x - 0.5f, y - 0.5f, z - 0.5f);
    glVertex3f(x, y, z);
    
}

void drawQuads(float p)
{
    glPushMatrix();
    
    float x = p;
    float y = p;
    float z = p;
    
    glBegin(GL_QUADS);
    /* Back side */
    glNormal3f(0.0f, 0.0f, -1.0f);
    vertex(0.0f, 0.0f, 0.0f);
    vertex(0.0f, y, 0.0f);
    vertex(x, y, 0.0f);
    vertex(x, 0.0f, 0.0f);
    
    /* Front side */
    glNormal3f(0.0f, 0.0f, 1.0f);
    vertex(0.0f, 0.0f, z);
    vertex(x, 0.0f, z);
    vertex(x, y, z);
    vertex(0.0f, y, z);
    
    /* Top side */
    glNormal3f(0.0f, 1.0f, 0.0f);
    vertex(0.0f, y, 0.0f);
    vertex(0.0f, y, z);
    vertex(x, y, z);
    vertex(x, y, 0.0f);
    
    /* Bottom side */
    glNormal3f(0.0f, -1.0f, 0.0f);
    vertex(0.0f, 0.0f, 0.0f);
    vertex(x, 0.0f, 0.0f);
    vertex(x, 0.0f, z);
    vertex(0.0f, 0.0f, z);
    
    /* Left side */
    glNormal3f(-1.0f, 0.0f, 0.0f);
    vertex(0.0f, 0.0f, 0.0f);
    vertex(0.0f, 0.0f, z);
    vertex(0.0f, y, z);
    vertex(0.0f, y, 0.0f);
    
    /* Right side */
    glNormal3f(1.0f, 0.0f, 0.0f);
    vertex(x, 0.0f, 0.0f);
    vertex(x, y, 0.0f);
    vertex(x, y, z);
    vertex(x, 0.0f, z);
    glEnd();
    
    glPopMatrix();
}


void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.75,0.75,-0.75,0.75,-5.5,5.5);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    
    glEnable(GL_DEPTH_TEST);

    initShaders();
}

void initShaders() {
    // Initialize shaders
    shaderProgram = initShader("./raycast.vp", "./raycast.fp");
//    passthroughProgram = initShader("./passThrough.vp", "./passThrough.fp");

    // Initialize buffers
    
    // Create frame buffer
    frameBuffer = genbindFrameBuffer();
    
    // Create backface texture
    backfaceTexture = genbindTexture2DRGBAF(width, height);
    
    // Attach the texture image to a framebuffer object... in this case to
    // GL_COLOR_ATTACHMENT0
    // 0 is the mipmap level (always)
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, backfaceTexture, 0);
    
    // Create screen rendering texture
    imageTexture = genbindTexture2DRGBAF(width, height);
    
    // Create render buffer
    renderBuffer = genbindRenderBuffer(width, height);
    
    setVolume();
    
    // Set uniforms
    uniform_tex = glGetUniformLocation(shaderProgram, "tex");
    uniform_volume_tex = glGetUniformLocation(shaderProgram, "volume_tex");
    uniform_stepSize = glGetUniformLocation(shaderProgram, "stepSize");
    uniform_emissivity = glGetUniformLocation(shaderProgram, "emissivity");
    uniform_scale = glGetUniformLocation(shaderProgram, "scale");
    uniform_epsilon = glGetUniformLocation(shaderProgram, "epsilon");
    uniform_drawBack = glGetUniformLocation(shaderProgram, "drawBack");
    uniform_drawRays = glGetUniformLocation(shaderProgram, "drawRays");
    uniform_drawFront = glGetUniformLocation(shaderProgram, "drawFront");
    uniform_rayDepth = glGetUniformLocation(shaderProgram, "rayDepth");
    
    printf("Shaders initialized successfully!\n");
}

static void reshape(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.75,0.75,-0.75,0.75,-5.5,5.5);
    
    glViewport(0, 0, w, h);
    glutPostRedisplay();
    
    // Update dimension
    width = w;
    height = h;
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

/*
 ** These functions implement a simple trackball-like motion control.
 */

float lastPos[3] = {0.0F, 0.0F, 0.0F};
int curx, cury;
int startX, startY;

void
trackball_ptov(int x, int y, int width, int height, float v[3])
{
    float d, a;
    
    /* project x,y onto a hemi-sphere centered within width, height */
    v[0] = (2.0F*x - width) / width;
    v[1] = (height - 2.0F*y) / height;
    d = (float) sqrt(v[0]*v[0] + v[1]*v[1]);
    v[2] = (float) cos((M_PI/2.0F) * ((d < 1.0F) ? d : 1.0F));
    a = 1.0F / (float) sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    v[0] *= a;
    v[1] *= a;
    v[2] *= a;
}


void
mouseMotion(int x, int y)
{
    float curPos[3], dx, dy, dz;
    
    trackball_ptov(x, y, width, height, curPos);
    if(trackingMouse)
    {
        dx = curPos[0] - lastPos[0];
        dy = curPos[1] - lastPos[1];
        dz = curPos[2] - lastPos[2];
        
        if (dx || dy || dz) {
            angle = 90.0F * sqrt(dx*dx + dy*dy + dz*dz);
            
            axis[0] = lastPos[1]*curPos[2] - lastPos[2]*curPos[1];
            axis[1] = lastPos[2]*curPos[0] - lastPos[0]*curPos[2];
            axis[2] = lastPos[0]*curPos[1] - lastPos[1]*curPos[0];
            
            lastPos[0] = curPos[0];
            lastPos[1] = curPos[1];
            lastPos[2] = curPos[2];
        }
    }
    glutPostRedisplay();
}

void
startMotion(int x, int y)
{
    
    trackingMouse = true;
    redrawContinue = false;
    startX = x; startY = y;
    curx = x; cury = y;
    trackball_ptov(x, y, width, height, lastPos);
	trackballMove=true;
}

void
stopMotion(int x, int y)
{
    
    trackingMouse = false;
    
    if (startX != x || startY != y) {
        redrawContinue = true;
    } else {
        angle = 0.0F;
        redrawContinue = false;
        trackballMove = false;
    }
}

void mouseButton(int button, int state, int x, int y)
{
    if(button==GLUT_RIGHT_BUTTON) exit(0);
    if(button==GLUT_MIDDLE_BUTTON) trackballMove = false;
    if(button==GLUT_LEFT_BUTTON) switch(state)
    {
        case GLUT_DOWN:
            y=height-y;
            startMotion( x,y);
            break;
        case GLUT_UP:
            stopMotion( x,y);
            break;
    }
}

int main(int argc, char** argv) {
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Simple Ray Tracer example");
    glutDisplayFunc(draw);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);

    // Initialize program
    init();
    
    glutMainLoop();
    
    return 0;
}