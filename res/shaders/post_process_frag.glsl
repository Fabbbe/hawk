#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform uint uTime;

float random(vec2 co);

void main() {
	vec3 result = vec3(0.0f);

	result.x = texture(screenTexture, vec2(TexCoords.x-0.01,TexCoords.y)).r;
	result.y = texture(screenTexture, TexCoords).g;
	result.z = texture(screenTexture, vec2(TexCoords.x+0.01,TexCoords.y)).b;

	//result += random( vec2( texture(screenTexture, TexCoords) ) + float(uTime)*0.0001);
	result += 0.025*random( vec2( texture(screenTexture, TexCoords) ) + float(uTime)*0.0001);
	//result += 0.1*random( vec2( TexCoords ) + float(uTime)*0.0001);
	FragColor = vec4(result, 1.0f);
}
float random(vec2 co) {
	return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}
