#ifndef SHADER_H
#define SHADER_H

#include "include/libs.h"

uint32_t createProgramVF(const char* vertexSourcePath, const char* fragmentSourcePath);

// Uniforms
void uniformMatrix4fv(uint32_t program, const char* uniformName, mat4 value);

#endif 
