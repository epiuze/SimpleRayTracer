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

void draw();
void drawQuads(float p);
void drawFullscreenQuad();

void renderBackFace();
void renderRayCast();
void vertex(float x, float y, float z);
void setTexture3DParam(int activeSamplerTexture);
void setVolume();

void init();

static void reshape(int w, int h);
static void keyboard(unsigned char key, int x, int y);


#endif
