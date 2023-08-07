#version 330 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
}fs_in;

out vec4 FragColor;

uniform sampler2D floorTexture;

void main() 
{
    FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}