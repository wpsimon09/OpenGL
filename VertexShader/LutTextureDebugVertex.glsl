#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoods;

void main()
{
	TexCoods = aTexCoords;
	gl_Position = vec4(aPos, 1.0);
}