#version 330 core

in vec2 aPos;
in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
	TexCoords = aTexCoords;
	gl_Position = vec4(aPos, 0.0f, 1.0f);
}
