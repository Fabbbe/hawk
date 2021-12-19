#version 130
in vec3 aPos;
in vec3 aNormal;
in vec2 aTex;

out vec3 vertNormal;
out vec3 vertPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0);

	// This is temporary so you can see the 3D object


	vertNormal = aNormal;
	vertPos = vec3(model * vec4(aPos, 1.0f));
}
