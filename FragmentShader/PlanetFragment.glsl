#version 330 core

in vec3 fColor;
in vec2 TexCoords;
in vec3 Normals;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform vec3 lightDir;
uniform vec3 lightColor;

void main()
{
	vec3 ambient = vec3(texture(texture_diffuse1, TexCoords)) * 0.2;

	vec3 lightDirection = normalize(-lightDir);

	vec3 normals = normalize(Normals);

	float diff = max(dot(normals, lightDirection), 0.0);

	vec3 diffuse = lightColor * diff * vec3(texture(texture_diffuse1, TexCoords));

	vec3 lightResult = ambient + diffuse;

	FragColor = vec4(lightResult , 1.0);
}