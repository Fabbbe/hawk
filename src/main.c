/* HAWK 
 * ====
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
 * char* items[itemCount]; 
 *
 * This somehow works, but this is not c99!
 * What I'm trying to say is DON'T WRITE THIS CODE!!!
 * It uses something akin to alloca() to do this, which could 
 * lead to unstable code.
 *
 * TODO
 * ----
 *
 * OBJ parser
 *   + Should be able to make vertex and index buffer
 *
 * Actually make 3D:
 *   + Send in the matricies
 *   + Get a better way to specify vertex attribs
 *
 * Make Uniform handling easier
 *   + uniform4f(basicShader, "playerColor", 1.0f, 1.0f, 1.0f, 1.0f);
 */

#include "include/libs.h"
#include "include/shader.h"
#include "include/object.h"

bool init();
void kill();

const uint32_t windowWidth = 640;
const uint32_t windowHeight = 480; SDL_Window* window = NULL;

int main(int argc, char* argv[]) {


	if (!init()) {
		fprintf(stderr, "Failed to init!\n");
		return -1;
	}

	// INIT OPENGL
	// ===========
	// TODO: finish this
	//

	
	SDL_GLContext context;
	context = SDL_GL_CreateContext(window);
	if (context == NULL) {
		fprintf(stderr, "Could not create context: %s\n", SDL_GetError());
	}

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Could not init GLEW: \n");
	}

	// OpenGL Vertecies
	// ----------------
	
	// Needed for opengl to work
	uint32_t vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// Shaders!
	// --------
	
	uint32_t basicShader;
	const char* vertexShaderPath = "./res/shaders/basic_vert.glsl";
	const char* fragmentShaderPath = "./res/shaders/basic_frag.glsl";
	basicShader = createProgramVF(vertexShaderPath, fragmentShaderPath);

	uint32_t uPlayerColor;
	uPlayerColor = glGetUniformLocation(basicShader, "playerColor");

	glUseProgram(basicShader);
	glUniform4f(uPlayerColor, 1.0f, 0.2f, 1.0f, 0.0f);
	glUseProgram(0);

	// Objects
	// -------
	
	Object square = readObject("./res/square.obj");

	// Process
	// =======
	
	bool running = true;
	float playerColor = 0.0f;
	
	while (running) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) { // Run through all the keys
					case SDLK_ESCAPE:
						running = false;
						break;
					default:
						break;
				}
			}
		}

		// This is a much better way to get the keystates in a game-like way
		const char* keyboardState = SDL_GetKeyboardState(NULL); 

		// Some color for a change 
		if (keyboardState[SDL_SCANCODE_W]) {
			playerColor += 0.004f;
		}
		if (keyboardState[SDL_SCANCODE_S]) {
			playerColor -= 0.004f;

		}
		if (playerColor < 0.0f)
			playerColor = 0.0f;
		else if (playerColor > 1.0f)
			playerColor = 1.0f;
				
		
		// Make uniform assigning easier
		glUseProgram(basicShader);
		glUniform4f(uPlayerColor, playerColor, playerColor, playerColor, 0.0f);
		glUseProgram(0);
		
		// Clear the Viewport
		// ==================
		
		glViewport(0, 0, windowWidth, windowHeight);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw the Triangles
		// ==================
		//
		// TODO: Make this readable, or abstract it out

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
