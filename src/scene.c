#include "include/scene.h"

/** 
 * findAttr:
 * @aNode: xmlNode to search
 * @attrName: the name of attribute to search for
 *
 * returns content of attribute, NULL if not found
 */
char* findAttr(xmlNode* aNode, const char* attrName) {
	for (xmlAttr* curAttr = aNode->properties; curAttr; curAttr = curAttr->next) {
		if (strcmp((const char*)curAttr->name, attrName) == 0) {
			return (char*)curAttr->children->content; 
		}
	}
	return NULL;
}

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

	// Find bounds

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

			if (curNode == scnRootElement) { 
				strcpy(scn->boundsPath, findAttr(curNode, "bounds"));
			}
			
			if (strcmp((const char*)curNode->name, "object") == 0) { 
				// curNode->properties contains what we need
				if (curNode->properties == NULL)
					fprintf(stderr, "Invalid object, no properties\n");
				else {
					if (findAttr(curNode, "transform") != NULL) {
						sscanf(
								findAttr(curNode, "transform"), 
								"%f,%f,%f", 
								&scn->objects[objectNum].x,
								&scn->objects[objectNum].y,
								&scn->objects[objectNum].z
						);
					} if (findAttr(curNode, "rotation") != NULL) {
						sscanf(
								findAttr(curNode, "rotation"), 
								"%f", 
								&scn->objects[objectNum].rot
						);
					} if (findAttr(curNode, "path") != NULL) {
						strcpy(scn->objects[objectNum].modelPath, findAttr(curNode, "path"));
					}
					//scn->objects[objectNum];
				}
				++objectNum;
			}
		} if (curNode->children != NULL) { // If it has children 
			curNode = curNode->children;
		}
	}

	if (scn->boundsPath != NULL) {
		scn->bounds = readMesh(scn->boundsPath);
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
	for (int i = 0; i < scn->objectCount; ++i) {
		freeObject(scn->objects[i].obj);
	}
	freeMesh(scn->bounds);
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
