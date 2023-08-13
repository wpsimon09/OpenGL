#version 330 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
}fs_in;

out vec4 FragColor;

uniform sampler2D woodCube;

uniform vec3 lightPos;
uniform vec3 lightColor;

void main() 
{
    //----------
    // AMBIENT
    //----------
    vec3 ambient = vec3(texture(woodCube, fs_in.TexCoords)* 0.02);
    
    //-----------
    // DIFFUSE
    //-----------
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diffStrength = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightColor * diffStrength * vec3(texture(woodCube, fs_in.TexCoords));
    
    vec3 result = diffuse + ambient;

    //-------------
    // FINAL RESULT
    //-------------    
    FragColor = vec4(result,1.0);
}