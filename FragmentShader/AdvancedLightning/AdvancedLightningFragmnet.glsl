#version 330 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLight;
    vec3 TangentViewPos;
    vec3 TangentLightPos;
    vec3 TangentFragPos;
    float hasNormalMap;
}fs_in;

out vec4 FragColor;

uniform sampler2D texture_diffuse0;
uniform sampler2D shadowMap;
uniform sampler2D texture_normal0;
uniform sampler2D texture_specular0;

uniform vec3 lightColor;

void main() 
{
    //----------
    // AMBIENT
    //----------
    vec3 ambient = vec3(texture(texture_diffuse0, fs_in.TexCoords)* 0.7);
    
    //--------
    // DIFFUSE
    //--------
    vec3 normal;
    if(fs_in.hasNormalMap == 1.0)
    {
        //sample normal vectors from the texture
        normal = texture(texture_normal0, fs_in.TexCoords).rgb;
        
        //convert from range [0,1] to the range [-1, 1]
        normal = normalize(normal * 2.0 - 1.0);    
    }
    else 
        normal = normalize(fs_in.Normal);

    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diffStrength = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightColor * diffStrength * vec3(texture(texture_diffuse0, fs_in.TexCoords));
    
    //--------
    //SPECULAR
    //--------
    vec3 texture_specular = lightColor;
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    float specStrength = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);

    specStrength = pow(max(dot(normal, halfwayDir), 0.0),64.0);
    vec3 specular = texture_specular * specStrength * vec3(texture(texture_specular0, fs_in.TexCoords));

    vec3 result = ambient + diffuse + specular;

    //-------------
    // FINAL RESULT
    //-------------    
    FragColor = vec4(result,1.0);
}