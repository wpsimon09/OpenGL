#version 330 core

in vec3 fColor;

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;

void main()
{
	float ambient = 0.6;
	FragColor = texture(texture_diffuse1, TexCoords) * ambient;
}