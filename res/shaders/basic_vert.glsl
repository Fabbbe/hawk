#version 330 core

in vec3 aPos;
in vec3 aNormal;
in vec2 aTex;

out vec3 vertNormal;
out vec3 vertPos;
out vec2 vertTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform uint uTime;

float random(vec2 co);

void main() {
	gl_Position = (projection * view * model * vec4(aPos, 1.0));

	// Multiplying with the model matrix correctly aligns everything with the light
	// Have to learn linear algebra at some point. I don't really 
	// understand the inverse(transpose(model)) part, only that I have to 
	// do it for the normals to translate correctly.

	vertNormal = vec3(inverse(transpose(model)) * vec4(aNormal, 1.0f));
	vertPos = vec3(model * vec4(aPos, 1.0f));
	vertTex = aTex;
}
float random(vec2 co) {
	return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}

