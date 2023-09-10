#version 330 core
layout (location = 0) in vec3 aPos;
//location 2 because of the createVAO function
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}