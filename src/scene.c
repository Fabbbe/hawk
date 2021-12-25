#include "include/scene.h"

/** 
 * loadScene:
 * @sceneFilePath: path to scene file
 *
 * Takes the path to a scene file, which is read as XML and returns 
 * pointer to a scene structure. This structure holds all objects and 
 * their transformations.
 *
 * The XML specifications can be read at doc/scene.md.
 *
 * TODO:
 *   + Read lights from the XML and add to a scene.
 */
Scene* loadScene(const char* sceneFilePath) {
	xmlDoc* scnFile;
	xmlNode* scnRootElement;
	Scene* scn = malloc(sizeof(Scene));

	// Initial values
	scn->objectCount = 0;
	scn->objects = NULL;
	
	// XML Reader
	// ==========
	scnFile = xmlReadFile(sceneFilePath, NULL, 0);

	if (scnFile == NULL) {
		fprintf(stderr, "Could not read scene file: %s\n", sceneFilePath);
		return scn;
	}

	scnRootElement = xmlDocGetRootElement(scnFile);

	for (xmlNode* curNode = scnRootElement; curNode; curNode = curNode->next) {
		if (curNode->type == XML_ELEMENT_NODE) {
			//printf("node type: Element, name: %s\n", curNode->name);
			
			// How many objects
			if (strcmp((const char*)curNode->name, "object") == 0) { 
				scn->objectCount += 1;
			}
		} if (curNode->children != NULL) { // If it has children 
			curNode = curNode->children;
		}
	}
	//printf("Object count: %u\n", scn->objectCount);

	// calloc is probably better here since it gives initial values
	//scn->objects = malloc(sizeof(struct SceneObject)*scn->objectCount);
	scn->objects = calloc(scn->objectCount, sizeof(struct SceneObject));
	
	// NEEDS TO BE SIMPLIFIED OR ABSTRACTED 
	// Too much spaghetti atm, getting kinda full
	uint32_t objectNum = 0;
	for (xmlNode* curNode = scnRootElement; curNode; curNode = curNode->next) {
		if (curNode->type == XML_ELEMENT_NODE) {
			
			// Read the object properties
			if (strcmp((const char*)curNode->name, "object") == 0) { 
				// curNode->properties contains what we need
				if (curNode->properties == NULL)
					fprintf(stderr, "Invalid object, no properties\n");
				else {
					for (xmlAttr* curAttr = curNode->properties; curAttr; curAttr = curAttr->next) {
						//printf("\%s: %s\n", curAttr->name, curAttr->children->content);
						if (strcmp((const char*)curAttr->name, "transform") == 0) {
							sscanf(
									(const char*)curAttr->children->content, 
									"%f,%f,%f", 
									&scn->objects[objectNum].x,
									&scn->objects[objectNum].y,
									&scn->objects[objectNum].z
							);
						} else if (strcmp((const char*)curAttr->name, "rotation") == 0) {
							sscanf(
									(const char*)curAttr->children->content, 
									"%f", 
									&scn->objects[objectNum].rot
							);
						} else if (strcmp((const char*)curAttr->name, "path") == 0) {
							sscanf(
									(const char*)curAttr->children->content, 
									"%s", 
									scn->objects[objectNum].modelPath
							);
						}
					}
					//scn->objects[objectNum];
				}
				++objectNum;
			}
		} if (curNode->children != NULL) { // If it has children 
			curNode = curNode->children;
		}
	}

	
	for (int i = 0; i < scn->objectCount; ++i) {
		scn->objects[i].obj = readObject(scn->objects[i].modelPath);
		glm_translate(
				scn->objects[i].obj->modelMatrix, 
				(vec3){scn->objects[i].x, scn->objects[i].y, scn->objects[i].z}
		); 
		glm_rotate_y(
				scn->objects[i].obj->modelMatrix, 
				glm_rad(scn->objects[i].rot), 
				scn->objects[i].obj->modelMatrix
		); 
	}

	// Load objects and do cool shit!

	return scn;
}

/**
 * destroyScene:
 * @scn: the sceen to free
 *
 * frees memory of scene, can't be used after this
 */
void destroyScene(Scene* scn) {
	free(scn->objects);
	free(scn);
}

/**
 * drawScene:
 * @program: the shader program to use
 * @scn: the scene to draw
 *
 * draws the objects in the scene
 */
void drawScene(uint32_t program, Scene* scn) {
	for (int i = 0; i < scn->objectCount; ++i) {
		drawObject(program, scn->objects[i].obj);
	}
}
