#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform float offSet;

void main() {
	gl_Position = vec4(aPos.x + offSet, aPos.y, aPos.z, 1.0f);
	ourColor = aPos;
}