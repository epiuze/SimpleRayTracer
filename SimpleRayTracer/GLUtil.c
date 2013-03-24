//
//  GLUtil.c
//  SimpleRayTracer
//
//  Created by Emmanuel Piuze-Phaneuf on 2013-03-23.
//  Copyright (c) 2013 Emmanuel Piuze-Phaneuf. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "GLUtil.h"

GLuint CLAMP_MODE = GL_CLAMP_TO_EDGE;
//	private int TEX_FILTER = GL2.GL_LINEAR;
GLuint TEX_ENV = GL_REPLACE;
//	private int TEX_ENV = GL2.GL_BLEND;

GLuint tmp1[1] = {};

// Generate and bin a frame buffer
GLuint genbindFrameBuffer() {
    
    glGenFramebuffers(1, tmp1);
    glBindFramebuffer(GL_FRAMEBUFFER, tmp1[0]);
    return tmp1[0];
}

// Generate and bind a render buffer
GLuint genbindRenderBuffer(GLuint width, GLuint height) {
    glGenRenderbuffers(1, tmp1);
    
    glBindRenderbuffer(GL_RENDERBUFFER, tmp1[0]);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, tmp1[0]);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return tmp1[0];
}

GLuint genbindTexture2D(GLuint activeSampler2D, GLuint width, GLuint height, GLuint internalFormat, GLuint format, GLuint type) {
    
    glActiveTexture(activeSampler2D);
    
    glGenTextures(1, tmp1);
    glBindTexture(GL_TEXTURE_2D, tmp1[0]);
    
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, 0);
    
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, TEX_ENV);
    
    GLuint filter = GL_NEAREST;
//    GLuint filter = GL_LINEAR;
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, CLAMP_MODE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, CLAMP_MODE);
    
    return tmp1[0];
}

GLuint genbindTexture2DRGBAF(GLuint activeSampler2D, GLuint width, GLuint height) {
    return genbindTexture2D(activeSampler2D, width, height, GL_RGBA16, GL_RGBA, GL_FLOAT);
}

