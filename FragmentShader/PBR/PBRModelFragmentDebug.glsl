#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLight;
    mat3 TBN;

    float hasNormalMap;
}fs_in;


void main()
{
	FragColor = vec4(1.0,0.0,0.0,1.0);
}