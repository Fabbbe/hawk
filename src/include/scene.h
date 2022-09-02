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

#define INTERACT_SCENE_LOAD 0b00000001
#define INTERACT_DIALOG     0b00000010
#define INTERACT_AUDIO      0b00000100

// Max number of lights in a scene
#define MAX_LIGHT_COUNT 4

typedef struct {
	// Event types:
	// 1: scene load. loads the scene from path in data.

	unsigned char type;
	char data[256];
} SceneEvent;

struct SceneObject {
	// translations of object
	float x;
	float y;
	float z;
	float rot; // rotate 

	char modelPath[PATH_LENGTH];

	Object3D* obj;
	SceneEvent* objectEvent;
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

	// Used to check if player inside level
	char boundsPath[PATH_LENGTH];
	Mesh3D* bounds;
	
	// Interactive meshes in level
	uint32_t interactCount;
	struct SceneInteract* interacts;
} Scene;

Scene* loadScene(const char* sceneFilePath);

void destroyScene(Scene* scn);

void drawScene(uint32_t program, Scene* scn);

#endif
