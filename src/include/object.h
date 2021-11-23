#ifndef OBJECT_H
#define OBJECT_H

#include "include/libs.h"

// All files should be read in as this
typedef struct {
	uint32_t vertCount;
	uint32_t indCount;
	uint32_t VBO; // ID for verteces
	uint32_t IBO; // ID for indices
	float* vertices;
	uint32_t* indices; 
} Object;

Object readObject(const char* objFilePath);

void freeObject(Object obj);

void drawObject(uint32_t program, Object obj);

#endif
