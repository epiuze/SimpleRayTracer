/* sets up flat mesh */
/* sets up elapsed time parameter for use by shaders */
/* uses a modified phong vertex shader and a pass through fragment shader */
/* vertex shader varies height of mesh sinusoidally */

#include <stdio.h>
#include <stdlib.h>
#include "ShaderUtil.h"

// program object id
GLuint program = 0;
GLint timeParam;

// buffer for error messages
GLchar *ebuffer;

// length of error message
GLsizei elength;

/* shader reader */
/* creates null terminated string from file */

char* readShaderSource(const char* shaderFile)
{
    FILE* fp = fopen(shaderFile, "r");
    char* buf;
    long size;
    
    if(fp==NULL) {
        printf("File not found...\n");
        return NULL;
    }
    else {
        printf("File found!\n");
    }
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    buf = (char*) malloc((size + 1) * sizeof(char));
    fread(buf, 1, size, fp);
    fclose(fp);
    return buf;
}

/* GLSL initialization */
GLuint initShader(const GLchar* vShaderFile, const GLchar* fShaderFile)
{
    GLint status;
    GLchar *vSource, *fSource;
    GLuint vShader, fShader;
    GLuint program;
    
    /* read shader files */
    
    vSource = readShaderSource(vShaderFile);
    if(vSource==NULL)
    {
        printf( "Failed to read vertex shader\n");
        exit(EXIT_FAILURE);
    }
    
    fSource = readShaderSource(fShaderFile);
    if(fSource==NULL)
    {
        printf("Failed to read fragment shader");
        exit(EXIT_FAILURE);
    }
    
    /* create program and shader objects */
    
    vShader = glCreateShader(GL_VERTEX_SHADER);
    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    program = glCreateProgram();
    
    /* attach shaders to the program object */
    
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    
    /* read shaders */
    
    glShaderSource(vShader, 1, (const GLchar**) &vSource, NULL);
    glShaderSource(fShader, 1, (const GLchar**) &fSource, NULL);
    
    /* compile vertex shader shader */
    
    glCompileShader(vShader);
    
    /* error check */
    
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &status);
    if(status==GL_FALSE)
    {
        printf("Failed to compile the vertex shader.\n");
        glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &elength);
        ebuffer = (GLchar *) malloc(elength*sizeof(char));
        glGetShaderInfoLog(vShader, elength, NULL, ebuffer);
        printf("%s\n", ebuffer);
        exit(EXIT_FAILURE);
    }
    
    /* compile fragment  shader shader */
    
    glCompileShader(fShader);
    
    /* error check */
    
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &status);
    if(status==GL_FALSE)
    {
        printf("Failed to compile the fragment shader.\n");
        glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &elength);
        ebuffer = (GLchar *) malloc(elength*sizeof(char));
        glGetShaderInfoLog(fShader, elength, NULL, ebuffer);
        printf("%s\n", ebuffer);
        exit(EXIT_FAILURE);
    }
    
    /* link  and error check */
    
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status==GL_FALSE)
    {
        printf("Failed to link program object.\n");
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &elength);
        ebuffer = (GLchar *) malloc(elength*sizeof(char));
        glGetProgramInfoLog(program, elength, &elength, ebuffer);
        printf("%s\n", ebuffer);
        exit(EXIT_FAILURE);
    }
    
    // let the caller handle the binding of the shader
//    glUseProgram(program);
    
    // Delete shaders
    glDeleteShader(fShader);
    glDeleteShader(vShader);

    // Done
    return program;
}

