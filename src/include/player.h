#ifndef PLAYER_H
#define PLAYER_H

#include "include/libs.h"
#include "include/object.h" // For mesh checking

typedef struct {
	vec3 playerPos; // used for physics
	vec3 playerVel; // used for physics
	vec3 cameraPos;
	float pitch;
	float yaw;
} Player;

Player* createPlayer();

void getPlayerView(Player* player, mat4 view);

void rotatePlayer(Player* player, float pitchDiff, float yawDiff);

//void movePlayer(Player* player, vec3 moveDir, bool sprinting);
void movePlayer(Player* player, Mesh3D* bounds, vec3 moveDir, bool sprinting, double deltaTime);

#endif 
