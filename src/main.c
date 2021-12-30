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
 * Rays to find what player is looking at
 *
 */

#include "include/libs.h"
#include "include/shader.h"
#include "include/object.h"
#include "include/scene.h"
#include "include/player.h"

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
	Scene* level = loadScene("res/scenes/level_01.scene"); // Next generation shit!

	// CGLM
	// ----
	mat4 view = GLM_MAT4_IDENTITY_INIT;
	mat4 projection = GLM_MAT4_IDENTITY_INIT;

	// Perspective
	glm_perspective(GLM_PI/2.3f, 
			(float)windowWidth / (float)windowHeight,
			0.1f,
			1000.0f,
			projection
	);

	uniformMatrix4fv(basicShader, "projection", projection);
	
	Player* mainPlayer = createPlayer();

	// Process
	// =======
	
	// player Positions
	
	uint64_t lastTime = 0;
	double deltaTime = 0;

	bool wireframe = false;
	bool fullscreen = false;
	
	bool running = true;
	while (running) {

		// Time handling
		{
			uint64_t curTime = SDL_GetPerformanceCounter();
			deltaTime = (double)((curTime - lastTime)*1000 / (double)SDL_GetPerformanceFrequency());
			lastTime = curTime;
			//printf("%lf\n", deltaTime);
		}

		// Event handling
		// ==============
		
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
			// Handle mousemotion
			if (event.type == SDL_MOUSEMOTION) {
				rotatePlayer(mainPlayer, (float)event.motion.xrel, (float)event.motion.yrel);
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
						if (!fullscreen) {
							SDL_DisplayMode dm;
							SDL_GetDesktopDisplayMode(0, &dm);
							SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
							SDL_SetWindowDisplayMode(window, &dm);
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

		vec3 moveDir;
		glm_vec3_zero(moveDir);
		{	// Keyboard state reading and input
			// This is a nice way of getting game-like input
			const unsigned char* keyboardState = SDL_GetKeyboardState(NULL); 

			// flying camera controls
			// I see a lot of people write this by watching for presses/releases 
			// of these buttons, but that looks like shit
			if (keyboardState[SDL_SCANCODE_W]) {
				moveDir[2] = 1.0f;
			}
			if (keyboardState[SDL_SCANCODE_A]) {
				moveDir[0] = 1.0f;
			}
			if (keyboardState[SDL_SCANCODE_S]) {
				moveDir[2] = -1.0f;
			}
			if (keyboardState[SDL_SCANCODE_D]) {
				moveDir[0] = -1.0f;
			}
		}

		glm_vec3_normalize(moveDir);
		movePlayer(mainPlayer, level->bounds, moveDir, true, deltaTime);

		// CAMERA MATH
		// ===========
		getPlayerView(mainPlayer, view);
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
