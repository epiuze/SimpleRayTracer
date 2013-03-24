//
//  ShaderUtil.h
//  SimpleRayTracer
//
//  Created by Emmanuel Piuze-Phaneuf on 2013-03-23.
//  Copyright (c) 2013 Emmanuel Piuze-Phaneuf. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#ifndef SimpleRayTracer_ShaderUtil_h
#define SimpleRayTracer_ShaderUtil_h

char* readShaderSource(const char* shaderFile);

/* GLSL initialization */
GLuint initShader(const GLchar* vShaderFile, const GLchar* fShaderFile);

#endif