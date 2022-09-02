#version 330 core

#define COLOR_AMOUNT 40.0
#define COLOR_SPREAD 0.002

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform uint uTime;

float random(vec2 co);

void main() {
	vec3 result = vec3(0.0f);

	result.x = texture(screenTexture, vec2(TexCoords.x - COLOR_SPREAD, TexCoords.y)).r;
	result.y = texture(screenTexture, vec2(TexCoords.x, TexCoords.y)).g;
	result.z = texture(screenTexture, vec2(TexCoords.x + COLOR_SPREAD, TexCoords.y)).b;

	//result = vec3(texture(screenTexture, TexCoords));

	// Remove Color
	result = floor(result*COLOR_AMOUNT)/COLOR_AMOUNT;

	FragColor = vec4(result, 1.0f);
	
}
float random(vec2 co) {
	return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}
