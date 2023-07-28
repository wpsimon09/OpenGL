#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

out VS_OUT 
{
	vec2 TexCoords;
} vs_out; //decalaring block 

void main() {
	vs_out.TexCoords = aTexCoords;    
    
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	gl_PointSize = gl_Position.z;

}