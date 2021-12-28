/* Everything that has to do with shader functionality should be 
 * contained within this file! We do as little as possible raw
 * OpenGL calls in the actual main.c
 */
#include "include/shader.h"

char* readShaderSource(const char* sourcePath) {
	/* Reads and returns the file content, don't forget to free */
	char* shaderSource;
	FILE* fp;

	fp = fopen(sourcePath, "r");
	if (fp == NULL) // Error reading file
		return NULL;
	
	// This tells the length of file
	fseek(fp, 0, SEEK_END);  
	int sourceLength = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	shaderSource = malloc(sizeof(char) * sourceLength + 1);
	fread(shaderSource, 1, sourceLength, fp);
	fclose(fp);

	shaderSource[sourceLength] = 0x00;

	return shaderSource;
}

uint32_t compileShader(const char* shaderSourcePath, uint32_t shaderType) {
	/* Compiles a shader from a given path to the shaders source code.
	 * shaderType is the type of the shader specified as a GLenum
	 *
	 * TODO: Error checking and abstraction
	 */
	uint32_t shader;
	char* shaderSource;


	shaderSource = readShaderSource(shaderSourcePath);

	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);

	free(shaderSource);

	int success;
	char infolog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infolog);
		fprintf(stderr, "Could not compile shader %s: %s\n", shaderSourcePath, infolog);
	}

	return shader;
}

uint32_t createProgramVF(const char* vertexSourcePath, const char* fragmentSourcePath) {
	/* Creates and returns the id of a OpenGL program.
	 *
	 * Takes the path to a vertex and fragment shader and returns an ID for 
	 * an OpenGL program.
	 *
	 */
	uint32_t programVF;
	uint32_t vertexShader;
	uint32_t fragmentShader;

	// Vertex Shader
	vertexShader = compileShader(vertexSourcePath, GL_VERTEX_SHADER);

	// Fragment Shader
	fragmentShader = compileShader(fragmentSourcePath, GL_FRAGMENT_SHADER);

	// Program
	programVF = glCreateProgram();
	glAttachShader(programVF, vertexShader);
	glAttachShader(programVF, fragmentShader);
	glLinkProgram(programVF);

	int success;
	char infolog[512];
	glGetProgramiv(programVF, GL_LINK_STATUS, &success);
	if(!success) {
		fprintf(stderr, "Could not link program\n");
		glGetProgramInfoLog(programVF, 512, NULL, infolog);
	}

	return programVF;
}

/**
 * createScreen:
 * @windowWidth: The width of the created screen
 * @windowHeight: The height
 * @scaleFactor: The scale of the pixelations
 *
 * Allocates all necessary buffers on the GPU
 */
Screen* createScreen(uint32_t windowWidth, uint32_t windowHeight, uint32_t scaleFactor) {
	Screen* scr;
	uint32_t screenWidth = windowWidth / scaleFactor;
	uint32_t screenHeight = windowHeight / scaleFactor;

	scr = (Screen*)malloc(sizeof(Screen));

	scr->program = createProgramVF(
			"./res/shaders/post_process_vert.glsl", 
			"./res/shaders/post_process_frag.glsl"
	);

	// Framebuffer Texture
	//
	glGenTextures(1, &scr->frameBufferTexID);
	glBindTexture(GL_TEXTURE_2D, scr->frameBufferTexID);
	glTexImage2D(GL_TEXTURE_2D, 
			0, GL_R3_G3_B2,
			screenWidth, screenHeight,
			0, GL_RGB,
			GL_UNSIGNED_BYTE, NULL
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// Renderbuffer
	//
	glGenRenderbuffers(1, &scr->renderBufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, scr->renderBufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Framebuffer
	//
	glGenFramebuffers(1, &scr->frameBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, scr->frameBufferID);
	glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
			GL_TEXTURE_2D, scr->frameBufferTexID, 
			0
	);
	glFramebufferRenderbuffer(
			GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 
			GL_RENDERBUFFER, scr->renderBufferID
	);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		fprintf(stderr, "Incomplete framebuffer\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	const float quadVertices[24] = { 
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenBuffers(1, &scr->vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, scr->vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*24, quadVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return scr;
}

void drawScreen(Screen* scr) {
	glDisable(GL_DEPTH_TEST);
	glUseProgram(scr->program);

	glBindBuffer(GL_ARRAY_BUFFER, scr->vertexBufferID);
	glBindTexture(GL_TEXTURE_2D, scr->frameBufferTexID);

	unsigned int vertPos = glGetAttribLocation(scr->program, "aPos");
	unsigned int texPos = glGetAttribLocation(scr->program, "aTexCoords");

	// VBO SHIT
	glVertexAttribPointer(
		vertPos,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(float)*4,
		(void*)0
	);
	glEnableVertexAttribArray(vertPos); 
	glVertexAttribPointer(
		texPos,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(float)*4,
		(void*)(2 * sizeof(float))
	);
	glEnableVertexAttribArray(texPos); 
	
	glDrawArrays(
		GL_TRIANGLES,
		0,
		6
	);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
}

/**
 * updateScreen:
 * @scr: the screen to update
 * @windowWidth: The new window width
 * @windowHeight: The new window height
 * @scaleFactor: The scale of the new pixelations
 *
 * Updates and rebinds all necessary buffers on the screen
 */
void updateScreen(Screen* scr, uint32_t windowWidth, uint32_t windowHeight, uint32_t scaleFactor) {
	uint32_t screenWidth = windowWidth / scaleFactor;
	uint32_t screenHeight = windowHeight / scaleFactor;

	glBindTexture(GL_TEXTURE_2D, scr->frameBufferTexID);
	glTexImage2D(GL_TEXTURE_2D, 
			0, GL_R3_G3_B2,
			screenWidth, screenHeight,
			0, GL_RGB,
			GL_UNSIGNED_BYTE, NULL
	);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, scr->renderBufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, scr->frameBufferID);
	glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
			GL_TEXTURE_2D, scr->frameBufferTexID, 
			0
	);
	glFramebufferRenderbuffer(
			GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 
			GL_RENDERBUFFER, scr->renderBufferID
	);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		fprintf(stderr, "Incomplete framebuffer\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * bindScreen:
 * scr: The screen which buffers to bind
 *
 * binds the screen framebuffer
 */
void bindScreen(Screen* scr) {
	glBindFramebuffer(GL_FRAMEBUFFER, scr->frameBufferID);
}

/**
 * unbindScreen:
 *
 * unbinds any screen and goes back to default framebuffer
 */
void unbindScreen() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * uniform;
 * @program: The shader program to set the uniform in.
 * @uniformName: The name of the uniform to set.
 * @value: The value to set the uniform to.
 *
 * There might also be a count in any uniform**v functions.
 */
void uniform1fv(uint32_t program, const char* uniformName, int count, float* value) {
	glUseProgram(program);
	uint32_t location = glGetUniformLocation(program, uniformName);
	glUniform1fv(location, count, value);
	glUseProgram(0);
}

void uniform3fv(uint32_t program, const char* uniformName, int count, float* value) {
	glUseProgram(program);
	uint32_t location = glGetUniformLocation(program, uniformName);
	glUniform3fv(location, count, value);
	glUseProgram(0);
}

void uniform1i(uint32_t program, const char* uniformName, int value) {
	glUseProgram(program);
	uint32_t location = glGetUniformLocation(program, uniformName);
	glUniform1i(location, value);
	glUseProgram(0);
}
void uniform1ui(uint32_t program, const char* uniformName, uint32_t value) {
	glUseProgram(program);
	uint32_t location = glGetUniformLocation(program, uniformName);
	glUniform1ui(location, value);
	glUseProgram(0);
}

void uniform1f(uint32_t program, const char* uniformName, float value) {
	glUseProgram(program);
	uint32_t location = glGetUniformLocation(program, uniformName);
	glUniform1f(location, value);
	glUseProgram(0);
}

void uniform2f(uint32_t program, const char* uniformName, vec2 value) {
	glUseProgram(program);
	uint32_t location = glGetUniformLocation(program, uniformName);
	glUniform2f(location, value[0], value[1]);
	glUseProgram(0);
}

void uniform3f(uint32_t program, const char* uniformName, vec3 value) {
	glUseProgram(program);
	uint32_t location = glGetUniformLocation(program, uniformName);
	glUniform3f(location, value[0], value[1], value[2]);
	glUseProgram(0);
}

void uniformMatrix4fv(uint32_t program, const char* uniformName, mat4 value) {
	glUseProgram(program);
	uint32_t location = glGetUniformLocation(program, uniformName);
	glUniformMatrix4fv(location, 1, GL_FALSE, (float*)value);
	glUseProgram(0);
}
