#version 330 core
layout (location = 0) out vec3 gPos;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorAndShinines;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    mat3 TBN;
    float hasNormalMap;
}fs_in;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform sampler2D texture_normal0;


void main()
{
   gPos = fs_in. FragPos;
   gNormal = normalize(fs_in.Normal);
   
   gColorAndShinines.rgb = texture(texture_diffuse0, fs_in.TexCoords).rgb;
   gColorAndShinines.a = 0.3;
}
