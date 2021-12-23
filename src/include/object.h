#ifndef OBJECT_H
#define OBJECT_H

#include "include/libs.h"
#include "include/shader.h"

#define PATH_LENGTH 256
// The base vertex
struct Vertex {
	
	// Vertices:
	float v0;
	float v1;
	float v2;

	// Normals:
	float n0;
	float n1;
	float n2;

	float t0;
	float t1;
};

// All files should be read in as this
typedef struct {
	uint32_t VBO; // ID for verteces
	uint32_t VAO;
	unsigned int vertexCount;
	struct Vertex* vertices;

	// Textures
	uint32_t texID;
	char texPath[PATH_LENGTH];
	
	// For OpenGL model uniform
	mat4 modelMatrix;
} Object3D;

Object3D* readObject(const char* objFilePath);

void freeObject(Object3D* obj);

void drawObject(uint32_t program, Object3D* obj);

#endif
