#include "include/object.h"

#define FAST_OBJ_IMPLEMENTATION 
#include "include/fast_obj.h" 

object3D readObject(const char* objFilePath) {
	/* Reads an object using fast_obj.h and returns the shit
	 *
	 * TODO:
	 *
	 */
	object3D obj;
	fastObjMesh* mesh; 


	// VAO Shit is here now??
	glGenVertexArrays(1, &obj.VAO);
	glBindVertexArray(obj.VAO);

	// Read the mesh
	mesh = fast_obj_read(objFilePath);

	obj.vertexCount = mesh->face_count*3; // Amount of vertices after being read
	obj.vertices = (struct vertex*)malloc(sizeof(struct vertex)*(obj.vertexCount));
	
	// Copy object into OpenGL
	
	// Vertices
	// --------
	
	// Should copy 1:1 the verticies to correct indices
	for (int i = 0; i < obj.vertexCount; ++i) {
		// Copy vertices:
		obj.vertices[i].v0 = mesh->positions[mesh->indices[i].p*3];
		obj.vertices[i].v1 = mesh->positions[mesh->indices[i].p*3+1];
		obj.vertices[i].v2 = mesh->positions[mesh->indices[i].p*3+2]; 

		// Copy normals:
		obj.vertices[i].n0 = mesh->normals[mesh->indices[i].n*3];
		obj.vertices[i].n1 = mesh->normals[mesh->indices[i].n*3+1];
		obj.vertices[i].n2 = mesh->normals[mesh->indices[i].n*3+2]; 

		// Copy texture coordinates:
		obj.vertices[i].t0 = mesh->texcoords[mesh->indices[i].t*2];
		obj.vertices[i].t1 = mesh->texcoords[mesh->indices[i].t*2+1];
	}

	glGenBuffers(1, &obj.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, obj.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct vertex) * obj.vertexCount, (float*) obj.vertices, GL_STATIC_DRAW);

	// Normals
	// -------
	
	fast_obj_destroy(mesh);
	return obj;
}

void freeObject(object3D obj) {
	// You could free right after handing over to the GPU
	free(obj.vertices);
}

void drawObject(uint32_t program, object3D obj) {
	/* Draws an object: 
	 * 
	 * TODO:
	 *
	 * Handle the glGetAttribLocation calls in shader.c:
	 *   + Get a shader 
	 */

	unsigned int vertexPos = glGetAttribLocation(program, "aPos");
	unsigned int normalPos = glGetAttribLocation(program, "aNormal");
	unsigned int texPos = glGetAttribLocation(program, "aTex");

	glUseProgram(program);

	// VBO SHIT
	glBindBuffer(GL_ARRAY_BUFFER, obj.VBO);

	glVertexAttribPointer(
		vertexPos,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(struct vertex),
		(void*)0
	);
	glEnableVertexAttribArray(vertexPos); 

	glVertexAttribPointer(
		normalPos,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(struct vertex),
		(void*)(sizeof(float)*3)
	);
	glEnableVertexAttribArray(normalPos); 

	glVertexAttribPointer(
		texPos,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(struct vertex),
		(void*)(sizeof(float)*6)
	);
	glEnableVertexAttribArray(texPos); 


	// IBO & DRAW

	glDrawArrays(
		GL_TRIANGLES,
		0,
		obj.vertexCount
	);

	glUseProgram(0);

	glDisableVertexAttribArray(vertexPos); 
	glDisableVertexAttribArray(normalPos); 
	glDisableVertexAttribArray(texPos); 
}
