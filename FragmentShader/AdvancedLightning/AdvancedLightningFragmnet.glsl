#version 330 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLight;
}fs_in;

out vec4 FragColor;

uniform sampler2D wood;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform bool blinnModel;

uniform vec3 specularColor;

float caclualteShadow(vec4 FragPosLight)
{
    //tranfsforms fragment position in ragne from [0, 1]
    vec3 projCoords = FragPosLight.xyz / FragPosLight.w;
    projCoords = projCoords * 0.5 + 0.5;

    //get the closest depth value from the shadow map
    //closest object to the light
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    
    //get the depth value of the current fragment 
    float currentDepth = projCoords.z;

    //compare if current depth value is bigger than the closest depth value
    // is true object is not in the shadow (1.0)
    // if false object is in the shadow (0.0)
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    return shadow;

}

void main() 
{
    //----------
    // AMBIENT
    //----------
    vec3 ambient = vec3(texture(wood, fs_in.TexCoords)* 0.02);
    
    //-----------
    // DIFFUSE
    //-----------
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diffStrength = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightColor * diffStrength * vec3(texture(wood, fs_in.TexCoords));
    
    //---------
    //SPECULAR
    //---------
    vec3 specularColor = lightColor;
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float specStrength = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    specStrength = pow(max(dot(normal, halfwayDir), 0.0),64.0);
    vec3 specular = specularColor * specStrength;
    
    //---------
    // SHADOWS
    //---------
    float shadow = caclualteShadow(fs_in.FragPosLight);
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * texture(wood, fs_in.TexCoords).rgb;

    //-------------
    // FINAL RESULT
    //-------------    
    FragColor = vec4(result,1.0);
}