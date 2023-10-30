#version 330 core
layout (location = 0)in vec3 aPos;

out vec3 localPos;

uniform mat4  projection, view;

void main()
{
	localPos = aPos;
	gl_Position = projection * view * vec4(localPos, 1.0);
}