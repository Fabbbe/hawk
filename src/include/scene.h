/* level.h
 *
 * This header is for the level.c file which will be used to load levels from 
 * XML files.
 *
 * I might need some way to categorize object for finding and bulk editing 
 * during gameplay.
 *
 */
#ifndef LEVEL_H
#define LEVEL_H

#include "include/libs.h"

#include "include/object.h"
//#include "include/shader.h"

#define PATH_LENGTH 256

// Max number of lights in a scene
#define MAX_LIGHT_COUNT 4

struct SceneObject {
	// translations of object
	float x;
	float y;
	float z;
	float rot; // rotate 

	char modelPath[PATH_LENGTH];

	Object3D* obj;
};

struct SceneLight {
	vec3 position;
	vec3 color;

	float constant;
	float linear;
	float quadratic;
};

typedef struct { // This is the main struct
	uint32_t objectCount;
	struct SceneObject* objects;

	uint32_t lightCount;
	struct SceneLight* lights;

	// navMesh might be a slight misnomer
	char boundsPath[PATH_LENGTH];
	Mesh3D* bounds;
} Scene;

Scene* loadScene(const char* sceneFilePath);

void destroyScene(Scene* scn);

void drawScene(uint32_t program, Scene* scn);

#endif
