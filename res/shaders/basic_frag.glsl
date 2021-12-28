#version 330 core

#define MAX_LIGHT_COUNT 4

in vec3 vertNormal;
in vec3 vertPos;
in vec2 vertTex;

out vec4 fragColor;

uniform sampler2D ourTexture;

struct PointLight {    
	vec3 position;
	vec3 color;

	// different strengths
	float constant;
	float linear;
	float quadratic;
};  

uniform int uPointLightCount;
uniform PointLight uPointLights[MAX_LIGHT_COUNT];

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 vertPos);

void main() {
	vec3 norm = normalize(vertNormal);

	// Diffuse
	// ======= 
	vec3 diffuse = vec3(0.0f);

	for (int i = 0; i < uPointLightCount; ++i) {
		diffuse += calculatePointLight(uPointLights[i], norm, vertPos);
	}


	// Ambient
	// =======

	// Combine them
	// ============
	vec3 result = diffuse * vec3(texture(ourTexture, vertTex));
	fragColor = vec4(result, 1.0f);
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 vertPos) {

	vec3 lightDir = normalize(light.position - vertPos);
	// Calculate diffuse
	float diff = max(dot(normal, lightDir), 0.0f);
	// Specular shading

	float distance = length(light.position - vertPos);
	float attenuation = 1.0f/(light.constant + light.linear*distance + light.quadratic*(distance*distance));

	vec3 diffuse = diff * light.color;
	vec3 ambient = light.color;  
	diffuse *= attenuation;
	ambient *= attenuation;

	return (diffuse + ambient);
}
