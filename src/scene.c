#include "include/scene.h"

/** 
 * findAttr:
 * @aNode: xmlNode to search
 * @attrName: the name of attribute to search for
 *
 * returns content of attribute, empty string if not found
 */
const char* findAttr(xmlNode* aNode, const char* attrName) {
	for (xmlAttr* curAttr = aNode->properties; curAttr; curAttr = curAttr->next) {
		if (strcmp((const char*)curAttr->name, attrName) == 0) {
			return (const char*)curAttr->children->content; 
		}
	}
	return "\0"; // should not be eaten by garbage collectors
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
	scn->lightCount = 0;
	scn->lights = NULL;
	
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
			else if (strcmp((const char*)curNode->name, "light") == 0) { 
				scn->lightCount += 1;
			}
		} if (curNode->children != NULL) { // If it has children 
			curNode = curNode->children;
		}
	}
	//printf("Light count: %u\n", scn->lightCount);

	// calloc is probably better here since it gives initial values
	scn->objects = calloc(scn->objectCount, sizeof(struct SceneObject));

	if (scn->lightCount > MAX_LIGHT_COUNT) 
		scn->lightCount = MAX_LIGHT_COUNT;
	scn->lights = calloc(MAX_LIGHT_COUNT, sizeof(struct SceneLight));
	
	uint32_t objectNum = 0;
	uint32_t lightNum = 0;
	for (xmlNode* curNode = scnRootElement; curNode; curNode = curNode->next) {
		if (curNode->type == XML_ELEMENT_NODE) {

			if (curNode == scnRootElement) { 
				strcpy(scn->boundsPath, findAttr(curNode, "bounds"));
			}
			
			if (strcmp((const char*)curNode->name, "object") == 0) { 
				// curNode->properties contains what we need
				if (curNode->properties == NULL)
					fprintf(stderr, "Invalid object, no properties\n");
				else { // sscanf returns EOF if it cant fill variables
					sscanf(
							findAttr(curNode, "transform"), 
							"%f,%f,%f", 
							&scn->objects[objectNum].x,
							&scn->objects[objectNum].y,
							&scn->objects[objectNum].z
					);
					sscanf(
							findAttr(curNode, "rotation"), 
							"%f", 
							&scn->objects[objectNum].rot
					);
					strcpy(scn->objects[objectNum].modelPath, findAttr(curNode, "path"));
					//scn->objects[objectNum];
				}
				++objectNum;
			}


			if ((strcmp((const char*)curNode->name, "light") == 0) && (lightNum < scn->lightCount)) { 

				// curNode->properties contains what we need
				if (curNode->properties == NULL)
					fprintf(stderr, "Invalid object, no properties\n");
				else {
					//printf("%s\n", findAttr(curNode, "position"));
					sscanf(
							findAttr(curNode, "position"), 
							"%f,%f,%f", 
							&scn->lights[lightNum].position[0],
							&scn->lights[lightNum].position[1],
							&scn->lights[lightNum].position[2]
					);
					sscanf(
							findAttr(curNode, "color"), 
							"%f,%f,%f", 
							&scn->lights[lightNum].color[0],
							&scn->lights[lightNum].color[1],
							&scn->lights[lightNum].color[2]
					);
					sscanf(
							findAttr(curNode, "constant"), 
							"%f", 
							&scn->lights[lightNum].constant
					);
					sscanf(
							findAttr(curNode, "linear"), 
							"%f", 
							&scn->lights[lightNum].linear
					);
					sscanf(
							findAttr(curNode, "quadratic"), 
							"%f", 
							&scn->lights[lightNum].quadratic
					);
					

				}
				++lightNum;
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

	// Default values
	for (int i = 0; i < scn->lightCount; ++i) { 
		if (scn->lights[lightNum].constant == 0.0f)
			scn->lights[lightNum].constant = 1.0f;
		if (scn->lights[lightNum].linear == 0.0f)
			scn->lights[lightNum].constant = 1.0f;
		if (scn->lights[lightNum].quadratic == 0.0f)
			scn->lights[lightNum].constant = 1.0f;
	}

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
	free(scn->lights);
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
	
	// Pass struct uniform4fv
	
	uniform1i(program, "uPointLightCount",  scn->lightCount);

	// This could be tidied up a bit
	uniform3f(program, "uPointLights[0].position", scn->lights[0].position);
	uniform3f(program, "uPointLights[1].position", scn->lights[1].position);
	uniform3f(program, "uPointLights[2].position", scn->lights[2].position);
	uniform3f(program, "uPointLights[3].position", scn->lights[3].position);

	uniform3f(program, "uPointLights[0].color", scn->lights[0].color);
	uniform3f(program, "uPointLights[1].color", scn->lights[1].color);
	uniform3f(program, "uPointLights[2].color", scn->lights[2].color);
	uniform3f(program, "uPointLights[3].color", scn->lights[3].color);

	uniform1f(program, "uPointLights[0].constant", scn->lights[0].constant);
	uniform1f(program, "uPointLights[1].constant", scn->lights[1].constant);
	uniform1f(program, "uPointLights[2].constant", scn->lights[2].constant);
	uniform1f(program, "uPointLights[3].constant", scn->lights[3].constant);

	uniform1f(program, "uPointLights[0].linear", scn->lights[0].linear);
	uniform1f(program, "uPointLights[1].linear", scn->lights[1].linear);
	uniform1f(program, "uPointLights[2].linear", scn->lights[2].linear);
	uniform1f(program, "uPointLights[3].linear", scn->lights[3].linear);

	uniform1f(program, "uPointLights[0].quadratic", scn->lights[0].quadratic);
	uniform1f(program, "uPointLights[1].quadratic", scn->lights[1].quadratic);
	uniform1f(program, "uPointLights[2].quadratic", scn->lights[2].quadratic);
	uniform1f(program, "uPointLights[3].quadratic", scn->lights[3].quadratic);

	for (int i = 0; i < scn->objectCount; ++i) {
		drawObject(program, scn->objects[i].obj);
	}
}
