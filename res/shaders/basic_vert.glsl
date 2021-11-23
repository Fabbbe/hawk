#version 130
in vec3 aPos;

out vec4 vertexColor;

uniform vec4 playerColor;

void main() {
	gl_Position = vec4(aPos, 1.0);
	vertexColor = playerColor;
}
