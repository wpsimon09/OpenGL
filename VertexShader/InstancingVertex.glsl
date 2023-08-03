#version 330 

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aOffSet;

out vec3 fColor;

out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main() {

	// calculate how small should each element be 
	vec2 pos = aPos * (gl_InstanceID / 100.0);
	
	// move the vertex to the desired postion
	// without indexing larga array
	gl_Position = vec4(pos + aOffSet, 0.0, 1.0);
	
	fColor = aColor;
}