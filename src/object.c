#include "include/object.h"

Object readObject(const char* objFilePath) {
	/* Reads the object and returns the indices and verices */
	Object obj;
	FILE* fp;

	int vertCount = 0;
	int indCount = 0;

	fp = fopen(objFilePath, "r");
	int c;
	
	// Count verticies and indicies
	while ((c = fgetc(fp)) != EOF) { 
		if (c == '\n') {
			c = fgetc(fp); // if newline get next char
			switch (c) {
				case 'v':
					++vertCount;
					break;
				case 'f':
					++indCount;
					break;
			}
		}
	}
	obj.vertCount = vertCount;
	obj.indCount = indCount;
	//printf("vertCount: %u\n", obj.vertCount);
	
	fseek(fp, 0, SEEK_SET);  // Roll back to beginning

	obj.vertices = calloc(obj.vertCount * 3, sizeof(float));
	obj.indices = calloc(obj.indCount * 3, sizeof(int));
	
	// Now we read the values into the buffers
	uint32_t vertIndex = 0;
	uint32_t indIndex = 0;
	while ((c = fgetc(fp)) != EOF) { 
		if (c == '\n') {
			c = fgetc(fp); // if newline get next char
			switch (c) {
				case 'v':
					fscanf(fp, " %f %f %f", 
							((float*)&obj.vertices[vertIndex]), 
							((float*)&obj.vertices[vertIndex + 1]), 
							((float*)&obj.vertices[vertIndex + 2])
					);
					vertIndex += 3;
					break;
				case 'f':
					fscanf(fp, " %u %u %u", 
							(uint32_t*)&obj.indices[indIndex],
							(uint32_t*)&obj.indices[indIndex + 1],
							(uint32_t*)&obj.indices[indIndex + 2]
					);
					indIndex += 3;
					break;
			}
		}
	}
	// Decrease all indices by 1
	
	for (int i = 0; i < obj.indCount * 3; ++i) {
		obj.indices[i] = obj.indices[i] - 1;
	}
	/* DEBUG PRINTS
	for (int i = 0; i < obj.indCount * 3; ++i) {
		printf(" %u", obj.indices[i]);
	}
	printf("\n");
	for (int i = 0; i < obj.vertCount * 3; ++i) {
		printf(" %f", obj.vertices[i]);
	}
	printf("\n");
	*/

	// Copy object into OpenGL
	
	glGenBuffers(1, &obj.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, obj.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * obj.vertCount * 3, obj.vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &obj.IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * obj.indCount * 3, obj.indices, GL_STATIC_DRAW);

	fclose(fp);
	return obj;
}

void freeObject(Object obj) {
	free(obj.vertices);
	free(obj.indices);
}

void drawObject(uint32_t program, Object obj) {
	/*
	 */
	glUseProgram(program);

	// VBO SHIT
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, obj.VBO);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		3*sizeof(float),
		(void*)0
	);

	// IBO & DRAW
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.IBO);
	glDrawElements(
			GL_TRIANGLES,
			obj.indCount*3,
			GL_UNSIGNED_INT,
			(void*)0
	);

	glUseProgram(0);
	glDisableVertexAttribArray(0);
}
