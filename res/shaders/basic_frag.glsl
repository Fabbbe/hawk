#version 130
out vec4 fragColor;

in vec4 vertColor;
in vec3 vertNormal;
in vec3 vertPos;
in vec2 vertTex;

uniform sampler2D ourTexture;

void main() {
	// Constants
	float ambientStrength = 0.1f;

	vec3 ambientColor =	vec3(1.0f,  1.0f, 1.0f);
	vec3 objectColor =	vec3(0.8f,  0.0f, 1.0f);
	vec3 lightPos =		vec3(20.0f, 0.0f, -40.0f);
	vec3 lightColor =	vec3(1.0f,  1.0f, 1.0f);

	// Diffuse
	// ======= 
	// Calculate normals
	vec3 norm = normalize(vertNormal);
	vec3 lightDir = normalize(lightPos - vertPos);

	// Calculate diffuse
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// Ambient
	// =======

	vec3 ambient = ambientStrength * ambientColor;

	// Combine them
	// ============
	vec3 result = (diffuse + ambient) * vec3(texture(ourTexture, vertTex));
	fragColor = vec4(result, 1.0f);
}
