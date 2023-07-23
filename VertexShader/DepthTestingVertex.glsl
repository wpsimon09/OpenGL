#version 330

layout (location = 0) in vec3 aPos;

//input of normal vectors
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 Position;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main() {

    //transforms normal coordniates to be correct in the world space because they are set in 
    //local space
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // get the position of the fragment
    Position = vec3(model * vec4(aPos, 1.0));

    //set positions of the vertex
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}