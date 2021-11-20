/* HAWK 
 * ====
 *
 * Some 3D graphics because I am bored in programming class
 * Python programming in uni is not fun ;-)
 *
 * Vulkan didn't work on a thinkpad x220 (who could've guessed)
 * But I'll write OpenGL to make up for it!
 *
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
 */

#include "include/libs.h"
#include "include/shader.h"

bool init();
void kill();
uint32_t createProgramVF(const char* vertexSourcePath, const char* fragmentSourcePath); // Vertex and Fragment shader

// Pointer to our window
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
	
	uint32_t vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	static const float vertexBufferData[9] = {
		-0.8f, -0.8f, -0.8f,
		0.8f, -0.8f, -0.8f,
		0.0f,  0.8f, -0.8f,
	};

	uint32_t vertexBufferID;

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

	// Shaders!
	// --------
	
	uint32_t basicShader;
	const char* vertexShaderPath = "./res/shaders/basic_vert.glsl";
	const char* fragmentShaderPath = "./res/shaders/basic_frag.glsl";
	basicShader = createProgramVF(vertexShaderPath, fragmentShaderPath);

	// DRAW & PROCESS
	// ==============
	
	bool running = true;
	
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
		
		// Clear the Viewport
		// ==================
		
		glViewport(0, 0, windowWidth, windowHeight);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw the Triangles
		// ==================
		//
		// TODO: Make this readable, or abstract it out

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			3*sizeof(float),
			(void*)0
		);

		glUseProgram(basicShader);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glUseProgram(0);
		glDisableVertexAttribArray(0);

		// Show it on the Window
		// =====================

		SDL_GL_SwapWindow(window);
	}

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
