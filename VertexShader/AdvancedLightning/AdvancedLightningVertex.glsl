#version 330 core

layout (location = 1) in vec3 aPos;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    
    vs_out.FragPos = aPos;
    vs_out.TexCoords = aTexCoords;
    vs_out.Normal = aNormal;

}
