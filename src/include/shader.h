#ifndef SHADER_H
#define SHADER_H

#include "include/libs.h"

typedef struct {
	uint32_t program;
	uint32_t frameBufferID;
	uint32_t frameBufferTexID;
	uint32_t vertexBufferID;
	uint32_t renderBufferID;
} Screen;

uint32_t createProgramVF(const char* vertexSourcePath, const char* fragmentSourcePath);

Screen* createScreen(uint32_t screenWidth, uint32_t screenHeight, uint32_t scaleFactor);
void drawScreen(Screen* src);

void updateScreen(Screen* scr, uint32_t windowWidth, uint32_t windowHeight, uint32_t scaleFactor);

void bindScreen(Screen* scr);
void unbindScreen();

// Uniforms
void uniform1fv(uint32_t program, const char* uniformName, int count, float* value);
void uniform3fv(uint32_t program, const char* uniformName, int count, float* value);

void uniform1i(uint32_t program, const char* uniformName, int value);
void uniform1ui(uint32_t program, const char* uniformName, uint32_t value);

void uniform1f(uint32_t program, const char* uniformName, float value);
void uniform2f(uint32_t program, const char* uniformName, vec2 value);
void uniform3f(uint32_t program, const char* uniformName, vec3 value);

void uniformMatrix4fv(uint32_t program, const char* uniformName, mat4 value);

#endif 
