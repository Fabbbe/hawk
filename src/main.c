/* HAWK
 * ====
 * v0.0.1
 * 
 * Copyright (c) 2021 Fabian Beskow 
 *
 * Some 3D graphics because I am bored in programming class
 * Python programming in uni is not fun ;-)
 *
 * I like the idea of rendering to a framebuffer and then 
 * using it as a texture so I can do some funky pixels
 *
 * A Quick Note on GCC
 * -------------------
 *
 * GCC lets the writer of a program programmatically allocate
 * the size of an arry:
 * 
 * int itemCount = 5;
 * char* items[itemCount * 3]; 
 *
 * Although this is c99 it might not work on all compilers and might be a bit 
 * funky when used. malloc is safer, but don't forget to free!
 *
 * TODO (ordered after priority):
 *
 * view bobbing:
 *   + Minecraft time!
 *   + Player position differs from the camera position
 *
 * Make the player -> ground relationship better
 *
 * Better movement
 *
 * Make lights a part of the scene file (1D Texture) 
 *
 * Raymarching to find what player is looking at
 *
 */

#include "include/libs.h"
#include "include/shader.h"
#include "include/object.h"
#include "include/scene.h"

#define INIT_WINDOW_HEIGHT 480
#define INIT_WINDOW_WIDTH 640

#define SCALE_FACTOR 2

#define ERROR_LOG_FILE_PATH "./error.log"

bool init();
void kill();

#ifndef NDEBUG
void debugCallback(GLenum source, GLenum type, GLuint id,
   GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
#endif

SDL_Window* window = NULL;

int main(int argc, char* argv[]) {
	SDL_GLContext context;

	uint32_t windowWidth = INIT_WINDOW_WIDTH;
	uint32_t windowHeight = INIT_WINDOW_HEIGHT; 

	// Allows stderr to write to file
	freopen(ERROR_LOG_FILE_PATH, "w", stderr);

	if (!init()) {
		fprintf(stderr, "Failed to init!\n");
		return -1;
	}
	
	// INIT OPENGL
	// ===========
	
	context = SDL_GL_CreateContext(window);
	if (context == NULL) {
		fprintf(stderr, "Could not create context: %s\n", SDL_GetError());
	}

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Could not init GLEW: \n");
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); // halves the amount of faces drawn :-)

#ifndef NDEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(debugCallback, NULL); 
#endif

	// Needed for opengl to work
	// This holds the configured vertex arrays in object.c
	uint32_t vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID); 


	// Framebuffer & Renderbuffer
	// --------------------------
	
	// Abstract this shit to shader or some shit

	Screen* screen = createScreen(INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT, SCALE_FACTOR);

	// Shaders!
	// --------
	// change to a struct!! :-)
	// ex: program basicShader = createProgramVF(...);
	uint32_t basicShader;
	basicShader = createProgramVF(
			"./res/shaders/basic_vert.glsl", 
			"./res/shaders/basic_frag.glsl"
	);

	// Objects
	// -------
	uint32_t startTime = SDL_GetTicks();
	Scene* level = loadScene("res/scenes/level_01.scene"); // Next generation shit!
	printf("Scene load time: %ums\n", SDL_GetTicks() - startTime);

	// CGLM
	// ----
	mat4 view = GLM_MAT4_IDENTITY_INIT;
	mat4 projection = GLM_MAT4_IDENTITY_INIT;

	vec3 cameraPos = {0.0f, 1.6f, 0.0f}; // 1.6 is eye level
	vec3 cameraUp = {0.0f, 1.0f, 0.0f};
	vec3 cameraDir = {0.0f, 0.0f, 0.0f};

	float pitch, yaw; // roll could be added at some point
	pitch = 0.0f;
	yaw = 0.0f;
	//roll = 0.0f;
	
	// Perspective
	glm_perspective(GLM_PI/2.3f, 
			(float)windowWidth / (float)windowHeight,
			0.1f,
			1000.0f,
			projection
	);

	// View
	glm_look(cameraPos,
			cameraDir,
			cameraUp,
			view
	);

	uniformMatrix4fv(basicShader, "view", view);
	uniformMatrix4fv(basicShader, "projection", projection);

	// Process
	// =======
	
	// player Positions

	bool wireframe = false;
	bool fullscreen = false;
	float moveSpeed = 0.04f;
	
	bool running = true;
	while (running) {

		// EVENT HANDLING
		// ==============
		
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
			// Handle mousemotion
			if (event.type == SDL_MOUSEMOTION) {
				yaw -= 0.0015f * (float)event.motion.xrel; // too large or small values will make it innacurate
				pitch -= 0.0015f * (float)event.motion.yrel; 
				if (pitch > GLM_PI / 2.1f) // Caps so you cant look too far up/down
					pitch = GLM_PI / 2.1f;
				else if (pitch < -GLM_PI / 2.1f)
					pitch = -GLM_PI / 2.1f;
			}

			// Handle keypresses
			else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) { // Run through all the keys
					case SDLK_ESCAPE:
						running = false;
						break;
					case SDLK_F4:
						if (SDL_GetRelativeMouseMode())
							SDL_SetRelativeMouseMode(SDL_FALSE);
						else
							SDL_SetRelativeMouseMode(SDL_TRUE);
						break;
					case SDLK_F8: // Reload scene
						destroyScene(level);
						level = loadScene("res/scenes/level_01.scene");
						break;
					case SDLK_F11:
						if (fullscreen) {
							windowWidth = INIT_WINDOW_WIDTH;
							windowHeight = INIT_WINDOW_HEIGHT;
							SDL_SetWindowSize(window, windowWidth, windowHeight);
							SDL_SetWindowFullscreen(window, true);
						}
						else
							SDL_SetWindowFullscreen(window, false);
						fullscreen = !fullscreen;
						break;
					case SDLK_TAB:
						if (wireframe) {
							glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
							wireframe = false;
						} else {
							glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
							wireframe = true;
						}
							
						break;
					default:
						break;
				}
			
			}
			else if (event.type == SDL_WINDOWEVENT) {
				switch (event.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
						windowWidth = event.window.data1;
						windowHeight = event.window.data2;
						glm_perspective(GLM_PI/2.3f, 
								(float)windowWidth / (float)windowHeight,
								0.1f,
								1000.0f,
								projection
						);
						uniformMatrix4fv(basicShader, "projection", projection);

						updateScreen(screen, windowWidth, windowHeight, SCALE_FACTOR);

						printf("Resized to %ux%u\n", windowWidth, windowHeight);
						break;
					default:
						break;
				}
			}
		}

		float playerVelX = 0.0f;
		float playerVelZ = 0.0f;
		{	// Keyboard state reading and input
			// This is a nice way of getting game-like input
			const unsigned char* keyboardState = SDL_GetKeyboardState(NULL); 

			// flying camera controls
			// I see a lot of people write this by watching for presses/releases 
			// of these buttons, but that looks like shit
			if (keyboardState[SDL_SCANCODE_W]) {
				playerVelZ += moveSpeed * cos(yaw);
				playerVelX += moveSpeed * sin(yaw);
			}
			if (keyboardState[SDL_SCANCODE_A]) {
				playerVelZ -= moveSpeed * sin(yaw);
				playerVelX += moveSpeed * cos(yaw);
			}
			if (keyboardState[SDL_SCANCODE_S]) {
				playerVelZ -= moveSpeed * cos(yaw);
				playerVelX -= moveSpeed * sin(yaw);
			}
			if (keyboardState[SDL_SCANCODE_D]) {
				playerVelZ += moveSpeed * sin(yaw);
				playerVelX -= moveSpeed * cos(yaw);
			}
			if (keyboardState[SDL_SCANCODE_LSHIFT]) {
				playerVelZ *= 1.8f;
				playerVelX *= 1.8f;
			}
		}
		
		// This could be done a lot better
		cameraPos[0] += playerVelX;
		if (!pointIsOverMesh(cameraPos, level->bounds, NULL)) {
			cameraPos[0] -= playerVelX * 1.02f; 
		}
		cameraPos[2] += playerVelZ;
		if (!pointIsOverMesh(cameraPos, level->bounds, NULL)) {
			cameraPos[2] -= playerVelZ * 1.02f;
		}

		// CAMERA MATH
		// ===========
		cameraDir[0] = sin(yaw) * cos(pitch); //x
		cameraDir[1] = sin(pitch); //y
		cameraDir[2] = cos(yaw) * cos(pitch); //z

		glm_look(cameraPos,
				cameraDir,
				cameraUp,
				view
		);
		uniformMatrix4fv(basicShader, "view", view);

		uniform1ui(screen->program, "uTime", SDL_GetTicks());
		// Clear the Viewport
		// ==================
		bindScreen(screen);
		glViewport(0, 0, windowWidth/SCALE_FACTOR, windowHeight/SCALE_FACTOR);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vertexArrayID); 
		glEnable(GL_DEPTH_TEST);
		drawScene(basicShader, level);

		unbindScreen();
		if (wireframe)
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glViewport(0, 0, windowWidth, windowHeight);
		drawScreen(screen);
		if (wireframe)
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

		// Show it on the Window
		// =====================
		SDL_GL_SwapWindow(window);
		//running = false;
	}

	destroyScene(level);
	//glDeleteFramebuffers(1, &frameBufferID);  

	// KILL
	// ====
	kill();
	return 0;
}

bool init() {
	/* Initializes SDL and saves the window to it's global variable */

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		fprintf(stderr, "Failed to init SDL: %s\n", SDL_GetError());
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#ifndef NDEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
	
	window = SDL_CreateWindow( "Hello Hawk!",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
	);

	if (window == NULL) {
		fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
		return false;
	}

	SDL_SetRelativeMouseMode(SDL_TRUE);

	return true;
}
void kill() {
	SDL_DestroyWindow(window);
	SDL_Quit();
}

// I could expand this
void debugCallback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	fprintf(stderr, "OpenGL Debug Message: %s\n", message);
}
// Thanks for reading
