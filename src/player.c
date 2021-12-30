#include "include/player.h"

/**
 * createPlayer:
 *
 * Creates a player
 */
Player* createPlayer() {
	Player* player;
	player = calloc(1, sizeof(Player)); // Init as zeros


	memcpy(player->playerPos, (vec3){0.0f, 1.6f, 0.0f}, sizeof(vec3));
	memcpy(player->cameraPos, (vec3){0.0f, 1.6f, 0.0f}, sizeof(vec3));
	
	return player;
}

/**
 * getPlayerView:
 * @player: player to get view from
 * @view: Matrix output
 *
 * generates and outputs view matrix
 */
void getPlayerView(Player* player, mat4 view) {
	vec3 cameraDir;
	cameraDir[0] = sin(player->yaw) * cos(player->pitch); //x
	cameraDir[1] = sin(player->pitch); //y
	cameraDir[2] = cos(player->yaw) * cos(player->pitch); //z
	glm_look(
			player->cameraPos,
			cameraDir,
			GLM_YUP,
			view
	);
}

void rotatePlayer(Player* player, float yawDiff, float pitchDiff) {
	player->yaw -= yawDiff     *0.0015;
	player->pitch -= pitchDiff *0.0015;
	if (player->pitch > GLM_PI / 2.1f) // Caps so you cant look too far up/down
		player->pitch = GLM_PI / 2.1f;
	else if (player->pitch < -GLM_PI / 2.1f)
		player->pitch = -GLM_PI / 2.1f;
}

void movePlayer(Player* player, Mesh3D* bounds, vec3 moveDir, bool sprinting, double deltaTime) {
	vec3 moveVec;
	float moveSpeed = 0.0025 * deltaTime;

	glm_vec3_rotate(moveDir, player->yaw, GLM_YUP);

	glm_vec3_scale(moveDir, moveSpeed, moveVec);
	//glm_vec3_add(moveVec, player->playerPos, player->playerPos);
	// This could be done a lot better
	player->playerPos[0] += moveVec[0];
	if (!pointIsOverMesh(player->playerPos, bounds, NULL)) {
		player->playerPos[0] -= moveVec[0] * 1.02f; 
	}
	player->playerPos[2] += moveVec[2];
	if (!pointIsOverMesh(player->playerPos, bounds, NULL)) {
		player->playerPos[2] -= moveVec[2] * 1.02f;
	}
	glm_vec3_copy(player->playerPos, player->cameraPos);

	// VIEW BOBBING!!!
	if (moveDir[1] != 0.0f || moveDir[2] != 0.0f) {
		player->cameraPos[1] += 0.015*sin((float)SDL_GetTicks()*0.013);
	}
}
