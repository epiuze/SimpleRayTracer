//
//  RayTracer.c
//  SimpleRayTracer
//
//  Created by Emmanuel Piuze-Phaneuf on 2013-03-23.
//  Copyright (c) 2013 Emmanuel Piuze-Phaneuf. All rights reserved.
//

#include "RayTracer.h"
#include "GLUtil.h"

#define DEFAULT_STEP_SIZE 0.005
#define DEFAULT_SCALE 3
#define DEFAULT_EMISSIVITY 0.15
#define DEFAULT_DEPTH 300

GLuint width = 500;
GLuint height= 500;

#define VOLUME_TEX_SIZE 128
int size = VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE* 4;

GLuint frameBuffer, renderBuffer;
GLuint volumeTexture, backfaceTexture, imageTexture;
GLuint activeSampler2D = GL_TEXTURE0;
GLuint volumeActiveSampler3D = GL_TEXTURE1;

GLuint shaderProgram;

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

void draw() {
    
    glClearColor(1, 1, 1, 1);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_LIGHTING);
    
    glPushMatrix();
    
    // Enable the render buffers
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    
    renderBackFace();
    renderRayCast();
    
    // Disable the render buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Render buffer to screen
    glEnable(GL_TEXTURE_2D);
    
    glActiveTexture(activeSampler2D);
    glBindTexture(GL_TEXTURE_2D, imageTexture);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    
    gluOrtho2D(0, 1, 0, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glDisable(GL_DEPTH_TEST);
    drawFullscreenQuad();
    glEnable(GL_DEPTH_TEST);
    
    glDisable(GL_TEXTURE_2D);
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    glPopMatrix();
    
    // Done!
    glutSwapBuffers();
}

void renderBackFace() {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, backfaceTexture, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    
    drawQuads(1);
    
    glDisable(GL_CULL_FACE);
}

//void setVolume() {
//    glActiveTexture(volumeActiveSampler3D);
//    
//    GLuint texture[1] = {};
//    glGenTextures(1, texture);
//    
//    glBindTexture(GL_TEXTURE_3D, texture[0]);
//
//    unsigned char buffer[tx][ty][tz] = {};
//    
//    // Fill with noise
////    srand(time(NULL));
////    for (int x = 0; x < tx; x++) {
////        for (int y = 0; y < ty; y++) {
////            for (int z = 0; z < tz; z++) {
////                buffer[x][y][z] = rand();
////            }
////        }
////    }
//    
//    // Need to set buffer
//    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, tx, ty, tz, 0, GL_RGBA, GL_UNSIGNED_BYTE, &buffer);
//    
//    setTexture3DParam(volumeActiveSampler3D);
//    
//    
//    volumeTexture = texture[0];
//}
// create a test volume texture, here you could load your own volume
void setVolume() {
//	unsigned char data[size] = {};
    unsigned char* data = (unsigned char *)malloc(size * sizeof(unsigned char));

    srand(time(NULL));
	for(int x = 0; x < VOLUME_TEX_SIZE; x++) {
        for(int y = 0; y < VOLUME_TEX_SIZE; y++) {
            for(int z = 0; z < VOLUME_TEX_SIZE; z++) {
                
                data[(x*4) + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4) + 0] = rand();
                data[(x*4) + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4) + 1] = rand();
                data[(x*4) + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4) + 2] = rand();
                data[(x*4) + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4) + 3] = rand();
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
    
}

void setTexture3DParam(int activeSamplerTexture) {
    
    glActiveTexture(activeSamplerTexture);
    
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
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
    glActiveTexture(volumeActiveSampler3D);
    glBindTexture(GL_TEXTURE_3D, volumeTexture);
    glBindSampler(0, volumeTexture);
    
    glActiveTexture(activeSampler2D);
    glBindTexture(GL_TEXTURE_2D, backfaceTexture);
    glBindSampler(2, backfaceTexture);
    
    // Default resolution:
    
    glUniform1f(uniform_stepSize, DEFAULT_STEP_SIZE);
    glUniform1f(uniform_scale, DEFAULT_SCALE);
    glUniform1f(uniform_epsilon, 0);
    glUniform1i(uniform_rayDepth, DEFAULT_DEPTH);
    glUniform1f(uniform_emissivity, DEFAULT_EMISSIVITY);
    
    glUniform1i(uniform_drawBack, 1);
    glUniform1i(uniform_drawFront, 0);
    glUniform1i(uniform_drawRays, 0);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    drawQuads(1.0f);
    
    glDisable(GL_CULL_FACE);
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
    glVertex3f(x - 0.5f, y - 0.5f, z - 0.5f);
    
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
    
    // Initialize buffers
    frameBuffer = genbindFrameBuffer();

    backfaceTexture = genbindTexture2DRGBAF(width, height, activeSampler2D);
    
    // Attach the texture image to a framebuffer object... in this case to
    // GL_COLOR_ATTACHMENT0
    // 0 is the mipmap level (always)
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_TEXTURE_2D, backfaceTexture, 0);

    imageTexture = genbindTexture2DRGBAF(width, height, activeSampler2D);
    
    renderBuffer = genbindRenderBuffer(width, height);
    
    setVolume();
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

int main(int argc, char** argv) {
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Simple Ray Tracer example");
    glutDisplayFunc(draw);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    
    // Initialize program
    init();
    
    // Initialize shaders
    shaderProgram = initShader("./raycast.vp", "./raycast.fp");
    
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

    
    glutMainLoop();
    
    return 0;
}