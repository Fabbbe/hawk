#version 130
in vec3 aPos;
in vec3 aNormal;
in vec2 aTex;

out vec3 vertNormal;
out vec3 vertPos;
out vec2 vertTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0);

	// Multiplying with the model matrix correctly aligns everything with the light
	vertNormal = vec3(model * vec4(aNormal, 1.0f));
	vertPos = vec3(model * vec4(aPos, 1.0f));
	vertTex = aTex;
}
