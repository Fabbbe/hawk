/* HAWK
 * ====
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
 * funky when used.
 *
 * TODO
 * ----
 *
 * Textures:
 *   + Load the textures
 *   + Send them to OpenGL
 *   + And then create a better way to do it in shader.c which is called by 
 *     object.c at object creation
 *
 * Make 3D Great Again:
 *   + Get a better way to specify vertex attribs (handled in drawObject atm)
 *
 */

#include "include/libs.h"
#include "include/shader.h"
#include "include/object.h"

bool init();
void kill();

const uint32_t windowWidth = 640;
const uint32_t windowHeight = 480; 
SDL_Window* window = NULL;

int main(int argc, char* argv[]) {


	if (!init()) {
		fprintf(stderr, "Failed to init!\n");
		return -1;
	}

	// INIT OPENGL
	// ===========
	
	SDL_GLContext context;
	context = SDL_GL_CreateContext(window);
	if (context == NULL) {
		fprintf(stderr, "Could not create context: %s\n", SDL_GetError());
	}

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Could not init GLEW: \n");
	}

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	SDL_SetRelativeMouseMode(SDL_TRUE);


	// OpenGL Vertecies
	// ----------------
	
	// Needed for opengl to work
	// This holds the configured vertex arrays in object.c
	uint32_t vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID); 

	// Shaders!
	// --------
	
	// change to a struct!! :-)
	// ex: program basicShader;
	uint32_t basicShader;
	const char* vertexShaderPath = "./res/shaders/basic_vert.glsl";
	const char* fragmentShaderPath = "./res/shaders/basic_frag.glsl";
	basicShader = createProgramVF(vertexShaderPath, fragmentShaderPath);

	// Objects
	// -------
	uint32_t beforeTime = SDL_GetTicks(); // Timer fun
	object3D square = readObject("./res/pumpkin.obj");
	printf("Object load time: %u ms\n",SDL_GetTicks() - beforeTime);

	// CGLM
	// ----
	mat4 model = GLM_MAT4_IDENTITY_INIT;
	mat4 view = GLM_MAT4_IDENTITY_INIT;
	mat4 projection = GLM_MAT4_IDENTITY_INIT;

	vec3 cameraPos = {0.0f, 0.0f, -100.0f};
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

	// model
	glm_scale(model, (vec3){1.0f, 1.0f, 1.0f});

	uniformMatrix4fv(basicShader, "model", model);
	uniformMatrix4fv(basicShader, "view", view);
	uniformMatrix4fv(basicShader, "projection", projection);

	// Process
	// =======
	

	bool wireframe = false;
	float moveSpeed = 0.6f;
	
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
				yaw -= 0.0005f * (float)event.motion.xrel; // too large or small values will make it innacurate
				pitch -= 0.0005f * (float)event.motion.yrel; 
				if (pitch > GLM_PI / 2.1f)
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
		}

		// This is a much better way to get the keystates in a game-like way
		const unsigned char* keyboardState = SDL_GetKeyboardState(NULL); 

		// flying camera controls
		// Q and E moves up and down
		if (keyboardState[SDL_SCANCODE_W]) {
			cameraPos[2] += moveSpeed * cos(yaw);
			cameraPos[0] += moveSpeed * sin(yaw);
		}
		if (keyboardState[SDL_SCANCODE_S]) {
			cameraPos[2] -= moveSpeed * cos(yaw);
			cameraPos[0] -= moveSpeed * sin(yaw);
		}
		if (keyboardState[SDL_SCANCODE_A]) {
			cameraPos[2] -= moveSpeed * sin(yaw);
			cameraPos[0] += moveSpeed * cos(yaw);
		}
		if (keyboardState[SDL_SCANCODE_D]) {
			cameraPos[2] += moveSpeed * sin(yaw);
			cameraPos[0] -= moveSpeed * cos(yaw);
		}
		if (keyboardState[SDL_SCANCODE_Q]) {
			cameraPos[1] += moveSpeed;
		}
		if (keyboardState[SDL_SCANCODE_E]) {
			cameraPos[1] -= moveSpeed;
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
		
		// Clear the Viewport
		// ==================
		glViewport(0, 0, windowWidth, windowHeight);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw the Triangles
		// ==================
		drawObject(basicShader, square);

		// Show it on the Window
		// =====================
		SDL_GL_SwapWindow(window);
	}

	freeObject(square);

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
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

#ifndef NDEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
	
	window = SDL_CreateWindow( "Hello Hawk!",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			windowWidth, windowHeight,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
			);

	if (window == NULL) {
		fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
		return false;
	}
	
	return true;
}
void kill() {
	SDL_DestroyWindow(window);
	SDL_Quit();
}

// Thanks for reading
