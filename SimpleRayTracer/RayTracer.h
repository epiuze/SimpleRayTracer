//
//  RayTracer.h
//  SimpleRayTracer
//
//  Created by Emmanuel Piuze-Phaneuf on 2013-03-23.
//  Copyright (c) 2013 Emmanuel Piuze-Phaneuf. All rights reserved.
//

#ifndef SimpleRayTracer_RayTracer_h
#define SimpleRayTracer_RayTracer_h

#include "ShaderUtil.h"
#include "GLUtil.h"

#define bool int
#define false 0
#define true 1

void draw();
void drawQuads(float p);
void drawFullscreenQuad();

void renderBackFace();
void renderRayCast();
void vertex(float x, float y, float z);

//void bindTexture(GLuint texture, GLuint textureUnit, GLuint textureType);
void bindTexture(GLuint texture, GLuint textureUnit, GLuint textureType, GLenum activeTexture);

void drawText(const char* text, int px, int py);

void setTexture3DParam();
void reshape_ortho(int w, int h);
static void reshape(int w, int h);

void setVolume();
void initShaders();
void renderScreen();
void stopMotion(int x, int y);
void startMotion(int x, int y);
void mouseButton(int button, int state, int x, int y);

void enable_renderbuffers();
void disable_renderbuffers();

void createVolume();

void init();

// Vars
bool drawBack, drawFront, drawRays;
bool rightMouseDown;

#endif
