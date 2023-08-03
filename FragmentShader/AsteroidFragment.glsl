#version 330 core

in vec2 TexCoordsAsteroids;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;

void main()
{
	float ambient = 0.6;
	FragColor = texture(texture_diffuse1, TexCoordsAsteroids) * ambient;
}