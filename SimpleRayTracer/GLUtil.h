//
//  GLUtil.h
//  SimpleRayTracer
//
//  Created by Emmanuel Piuze-Phaneuf on 2013-03-23.
//  Copyright (c) 2013 Emmanuel Piuze-Phaneuf. All rights reserved.
//

#ifndef SimpleRayTracer_GLUtil_h
#define SimpleRayTracer_GLUtil_h

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

extern GLuint tmp1[1];

GLuint genbindFrameBuffer();

// Generate and bind a render buffer
GLuint genbindRenderBuffer(GLuint width, GLuint height);

GLuint genbindTexture2D(GLuint width, GLuint height, GLuint internalFormat, GLuint format, GLuint type);

GLuint genbindTexture2DRGBAF(GLuint width, GLuint height);

#endif
