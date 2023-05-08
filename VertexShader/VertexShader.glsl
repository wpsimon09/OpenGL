#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;
out vec4 fragColor;

uniform float offSet;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
vec3 FragPos;


void main() {
	gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0f);
	FragPos = vec3(model * vec4(aPos, 1.0));
	TexCoord = aTexCoord;

	vec3 normal = normalize(aNormal);

	//ambientl
	float ambientStrength = 0.7f;

	vec3 ambient = ambientStrength * lightColor;

	//diffuse
	vec3 lightDir = normalize(lightPos - FragPos);

	float diff = max(dot(normal, lightDir), 0);

	vec3 diffuse = diff * lightColor;


	//specular
	float specularStrength = 0.2f;

	vec3 viewDireciton = normalize(viewPos - FragPos);

	vec3 reflectDir = reflect(-lightDir, normal);

	float spec = pow(max(dot(viewDireciton, reflectDir), 0),64);

	vec3 specular = specularStrength * spec * lightColor;

	fragColor =vec4((ambient + diffuse + spec) * objectColor, 1.0f);
}