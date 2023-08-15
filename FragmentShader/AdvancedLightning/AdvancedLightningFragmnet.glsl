#version 330 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
}fs_in;

out vec4 FragColor;

uniform sampler2D floorTexture;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform bool blinnModel;
uniform vec3 specularColor;

void main() 
{
    //----------
    // AMBIENT
    //----------
    vec3 ambient = vec3(texture(floorTexture, fs_in.TexCoords)* 0.1);

    //-----------
    // DIFFUSE
    //-----------
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diffStrength = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightColor * diffStrength * vec3(texture(floorTexture, fs_in.TexCoords));

    //---------
    //SPECULAR
    //---------
    vec3 specularColor = lightColor;
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float specStrength = 0.0;
    if (blinnModel) 
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        specStrength = pow(max(dot(normal, halfwayDir), 0.0),16.0);
    }
    else 
    {
        vec3 reflectionVector = reflect(-lightDir, normal);
        specStrength = pow(max(dot(viewDir, reflectionVector), 0.0),32.0);

    }
    vec3 specular = specularColor * specStrength;


    //-------------
    // FINAL RESULT
    //-------------
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result,1.0);
}
