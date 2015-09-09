#ifndef RENDERER_HPP__
#define RENDERER_HPP__

#include "globals.hpp"
#include "renderer.hpp"

namespace renderer
{

extern std::string fsHeader;
extern std::string fsFooter;


GLuint createTexture(GLint format, unsigned int w, unsigned int h, const GLvoid * data);
GLuint createTexture(const GLvoid *data, GLint format, unsigned int w, unsigned int h, GLint internalFormat, GLint scaling, GLint repeat);
GLuint createTexture(const std::string& file, GLint internalFormat, GLint scaling, GLint repeat);

GLuint compileShader(GLenum shaderType, const char *shader);
GLuint compileAndLinkProgram(const char *vertexShader, const char *fragmentShader);
std::string createShader(const std::string &file);

void unloadTextures();
GLint loadTexture(int number);

}

#endif 
