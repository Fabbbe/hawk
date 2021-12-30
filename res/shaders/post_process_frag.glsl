#version 330 core

#define COLOR_AMOUNT 17.0
#define COLOR_SPREAD 0.007

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform uint uTime;

float random(vec2 co);

void main() {
	vec3 result = vec3(0.0f);

	result.x = texture(screenTexture, vec2(TexCoords.x-COLOR_SPREAD,TexCoords.y)).r;
	result.y = texture(screenTexture, TexCoords).g;
	result.z = texture(screenTexture, vec2(TexCoords.x+COLOR_SPREAD,TexCoords.y)).b;

	//result = vec3(texture(screenTexture, TexCoords));

	// Remove some color
	result.x = floor(result.x*COLOR_AMOUNT)/COLOR_AMOUNT;
	result.y = floor(result.y*COLOR_AMOUNT)/COLOR_AMOUNT;
	result.z = floor(result.z*COLOR_AMOUNT)/COLOR_AMOUNT;

	// Light Noise
	vec3 expBrightness = pow(vec3(170.0), vec3(texture(screenTexture, TexCoords)) - 1.0);
	result += expBrightness * 0.6*random( vec2(texture(screenTexture, TexCoords)) + float(uTime)*0.0001);

	// Scan lines
	result += 0.01 * (1.0+sin(160.0*TexCoords.y+float(uTime)*0.03))/2.0;

	FragColor = vec4(result, 1.0f);
}
float random(vec2 co) {
	return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}
