#include "include/object.h"

#define FAST_OBJ_IMPLEMENTATION 
#include "include/fast_obj.h" 

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

// Use this for any missing texture
// This file MUST exist for program to run
#define ERROR_IMAGE_PATH "./res/error.png\0"


/**
 * pointIsOverMesh:
 * @point: point to check if over mesh
 * @mesh: the mesh to check
 * @distance: pointer to distance value for retrurning
 *
 * Checks wether point is over the mesh and returns bool. the distance value 
 * is also changed if true;
 */
bool pointIsOverMesh(vec3 point, Mesh3D* mesh, float* distance) {
	bool isOver = false;
	for (int i=0; i < mesh->vertexCount/3 ; ++i) {
		// Triangle vertices
		vec3 v0;
		vec3 v1;
		vec3 v2;

		vec3 down = {0.0f, -1.0f, 0.0f};

		v0[0] = mesh->vertices[i*3].v0;
		v0[1] = mesh->vertices[i*3].v1;
		v0[2] = mesh->vertices[i*3].v2;
		
		v1[0] = mesh->vertices[i*3+1].v0;
		v1[1] = mesh->vertices[i*3+1].v1;
		v1[2] = mesh->vertices[i*3+1].v2;

		v2[0] = mesh->vertices[i*3+2].v0;
		v2[1] = mesh->vertices[i*3+2].v1;
		v2[2] = mesh->vertices[i*3+2].v2;

		if (glm_ray_triangle(point, down, v0, v1, v2, distance)){
			isOver = true;
			//break;
		}
	}
	return isOver;
}

/**
 * readMesh:
 * @meshFilePath: Path to the .obj file
 *
 * reads a 3D mesh from a file.
 */
Mesh3D* readMesh(const char* meshFilePath) {
	Mesh3D* mesh = (Mesh3D*)malloc(sizeof(Mesh3D));
	mesh->vertices = NULL;

	fastObjMesh* foMesh; 

	foMesh = fast_obj_read(meshFilePath);
	if (mesh == NULL) {
		fprintf(stderr, "Could not load object '%s'\n", meshFilePath);
		return mesh;
	}

	mesh->vertexCount = foMesh->face_count*3; // Amount of vertices after being read
	mesh->vertices = (struct MeshVertex*)malloc(sizeof(struct MeshVertex)*(mesh->vertexCount));

	for (int i = 0; i < mesh->vertexCount; ++i) {
		// Copy vertices:
		mesh->vertices[i].v0 = foMesh->positions[foMesh->indices[i].p*3];
		mesh->vertices[i].v1 = foMesh->positions[foMesh->indices[i].p*3+1];
		mesh->vertices[i].v2 = foMesh->positions[foMesh->indices[i].p*3+2]; 
	}

	fast_obj_destroy(foMesh);

	return mesh;
}

/**
 * readObject:
 * @objFilePath: Path to the .obj file
 *
 * reads an object from file
 */
Object3D* readObject(const char* objFilePath) {

	Object3D* obj = (Object3D*)malloc(sizeof(Object3D)); // Small malloc
	fastObjMesh* mesh; 

	// Default value
	obj->vertices = NULL;
	
	// Since initializing any other way would be a pain
	glm_mat4_identity(obj->modelMatrix);

	// VAO Shit 
	glGenVertexArrays(1, &obj->VAO);
	glBindVertexArray(obj->VAO);

	// Read the mesh
	// This needs to be done before the texture loading since it reads the .mtl
	mesh = fast_obj_read(objFilePath);
	if (mesh == NULL) {
		fprintf(stderr, "Could not load object '%s'\n", objFilePath);
		return obj;
	}

	// Texture
	// -------
	
	// OpenGL Texture buffers
	glGenTextures(1, &obj->texID);  
	glBindTexture(GL_TEXTURE_2D, obj->texID);  

	// OpenGL buffer settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	// First we need to find texture paths:
	memset(obj->texPath, '\0', sizeof(obj->texPath));
	if (mesh->materials != NULL) {
		strcpy(obj->texPath, mesh->materials[0].map_Kd.path); 
	}
	else {
		strcpy(obj->texPath, ERROR_IMAGE_PATH);
	}

	//printf("Texture Path: %s\n", obj->texPath);
	// Load image
	stbi_set_flip_vertically_on_load(true); // For OpenGL compatability
	int texWidth, texHeight, nrChannels;
	unsigned char *data = stbi_load(obj->texPath, &texWidth, &texHeight, &nrChannels, 0); 
	if (data == NULL) { // All imported textures should have ONLY RGB
		fprintf(stderr, "Could not open '%s'\n", obj->texPath);
		strcpy(obj->texPath, ERROR_IMAGE_PATH);
		data = stbi_load(obj->texPath, &texWidth, &texHeight, &nrChannels, 0); 
	}
	glTexImage2D(GL_TEXTURE_2D, 
			0, GL_RGB, 
			texWidth, texHeight, 
			0, GL_RGB, 
			GL_UNSIGNED_BYTE, 
			data
	);

	stbi_image_free(data);

	// Vertices
	// --------

	obj->vertexCount = mesh->face_count*3; // Amount of vertices after being read
	obj->vertices = (struct Vertex*)malloc(sizeof(struct Vertex)*(obj->vertexCount));

	
	// Should copy 1:1 the verticies to correct indices
	//
	// This is probably one of the fastest ways to do this copying since 
	// methods like memcpy wouldn't work since we have to interpolate the new 
	// array.
	for (int i = 0; i < obj->vertexCount; ++i) {
		// Copy vertices:
		obj->vertices[i].v0 = mesh->positions[mesh->indices[i].p*3];
		obj->vertices[i].v1 = mesh->positions[mesh->indices[i].p*3+1];
		obj->vertices[i].v2 = mesh->positions[mesh->indices[i].p*3+2]; 

		// Copy normals:
		obj->vertices[i].n0 = mesh->normals[mesh->indices[i].n*3];
		obj->vertices[i].n1 = mesh->normals[mesh->indices[i].n*3+1];
		obj->vertices[i].n2 = mesh->normals[mesh->indices[i].n*3+2]; 

		// Copy texture coordinates:
		obj->vertices[i].t0 = mesh->texcoords[mesh->indices[i].t*2];
		obj->vertices[i].t1 = mesh->texcoords[mesh->indices[i].t*2+1];
	}

	// Copy the generated buffers to OpenGL
	glGenBuffers(1, &obj->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, obj->VBO);
	glBufferData(
			GL_ARRAY_BUFFER, 
			sizeof(struct Vertex) * obj->vertexCount, 
			(float*) obj->vertices, 
			GL_STATIC_DRAW
	);
	fast_obj_destroy(mesh);

	return obj;
}


/**
 * freeObject:
 * @obj: the object to free
 *
 * frees an object, which becomes unusable afterwards
 */
void freeObject(Object3D* obj) {
	// You could free right after handing over to the GPU
	free(obj->vertices);
	free(obj);
}
void freeMesh(Mesh3D* mesh) {
	free(mesh->vertices);
	free(mesh);
}

/**
 * drawObject:
 * @program: the shader program to use
 * @obj: the object to draw
 *
 * draws an object to the screen
 *
 * TODO:
 * Fixing Shaders:
 *   + Create some sort of shader struct
 *   + Also functions to handle that struct in shader.c
 */
void drawObject(uint32_t program, Object3D* obj) {

	// This is necessary to do at the beginning
	uniformMatrix4fv(program, "model", obj->modelMatrix);

	unsigned int vertexPos = glGetAttribLocation(program, "aPos");
	unsigned int normalPos = glGetAttribLocation(program, "aNormal");
	unsigned int texPos = glGetAttribLocation(program, "aTex");

	glUseProgram(program);

	// VBO SHIT
	glBindBuffer(GL_ARRAY_BUFFER, obj->VBO);

	glVertexAttribPointer(
		vertexPos,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(struct Vertex),
		(void*)0
	);
	glEnableVertexAttribArray(vertexPos); 

	glVertexAttribPointer(
		normalPos,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(struct Vertex),
		(void*)(sizeof(float)*3)
	);
	glEnableVertexAttribArray(normalPos); 

	glVertexAttribPointer(
		texPos,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(struct Vertex),
		(void*)(sizeof(float)*6)
	);
	glEnableVertexAttribArray(texPos); 

	// Texture
	glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
	glBindTexture(GL_TEXTURE_2D, obj->texID);

	// Model uniform
	//

	// Draw call
	glDrawArrays(
		GL_TRIANGLES,
		0,
		obj->vertexCount
	);

	glUseProgram(0);

	glDisableVertexAttribArray(vertexPos); 
	glDisableVertexAttribArray(normalPos); 
	glDisableVertexAttribArray(texPos); 
}
