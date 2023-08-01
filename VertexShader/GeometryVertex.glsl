#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; //get the normals from the model


out VS_OUT{
	out vec3 normal;
}vs_out;

uniform mat4 view;
uniform mat4 model;

void main() {    
    gl_Position = view * model * vec4(aPos, 1.0);

	//multiply normal vectors with the noraml matrix in order to corectly display them on the screen
	mat3 normalMatrix = mat3(transpose(inverse(view * model)));
	vs_out.normal = normalize(vec3(vec4(normalMatrix * aNormal, 0.0)));
}