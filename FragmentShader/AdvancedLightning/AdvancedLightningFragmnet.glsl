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
uniform sampler2D heightMap;

uniform vec3 lightColor;

uniform vec3 texture_specular0;

uniform float heightScale;

float caclualteShadow(vec4 FragPosLight, float bias)
{
    //tranfsforms fragment position in ragne from [0, 1]
    vec3 projCoords = FragPosLight.xyz / FragPosLight.w;
    projCoords = projCoords * 0.5 + 0.5;

    //get the closest depth value from the shadow map
    //closest object to the light
    float closestDepth = texture(shadowMap, projCoords.xy).w;
    
    //get the depth value of the current fragment 
    float currentDepth = projCoords.z;  

    //compare if current depth value is bigger than the closest depth value
    // is true object is not in the shadow (1.0)
    // if false object is in the shadow (0.0)
    float shadow = 0;

    //this will be used for sampling neiborough texels in mipmap level 0
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    // creates 3x3 grid around the sampled texel
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            //sample the surounding texel
            //the multiplication by texelSize is necesary since the shadow map is in different resolution
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    //calculate the average 
    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;   

    return shadow;

}

vec2 ParalaxMapping(vec2 textureCoordinates, vec3 viewDir)
{
    float numOfLayers = 10;

    float layerDepth = 1 / numOfLayers;

    float currentLyerDepth  = 0.0f;

    vec2 P = viewDir.xy * heightScale;
    vec2 deltaTexCoords = P / numOfLayers;
    
    vec2 currentTexCoords = textureCoordinates;

    float currentDepthMapValue  = texture(heightMap, currentTexCoords).r;

    while(currentLyerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(heightMap, currentTexCoords).r;
        currentLyerDepth += layerDepth;
    }

    return currentTexCoords;
}

void main() 
{
    //----------------
    // PARALAX MAPPING
    //----------------

    //calculate the vector V vector
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    
    //offset the texture coordniates based on the heightMap
    vec2 texCoords = ParalaxMapping(fs_in.TexCoords, viewDir); 
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 ||
        texCoords.y < 0.0)
        discard;

    //----------
    // AMBIENT
    //----------
    vec3 ambient = vec3(texture(texture_diffuse0, texCoords)* 0.4);
    
    //--------
    // DIFFUSE
    //--------
    vec3 normal;
    if(fs_in.hasNormalMap == 1.0)
    {
        //sample normal vectors from the texture
        normal = texture(texture_normal0, texCoords).rgb;
        
        //convert from range [0,1] to the range [-1, 1]
        normal = normalize(normal * 2.0 - 1.0);    
    }
    else 
        normal = normalize(fs_in.Normal);

    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diffStrength = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightColor * diffStrength * vec3(texture(texture_diffuse0, texCoords));
    
    //--------
    //SPECULAR
    //--------
    vec3 texture_specular = lightColor;
    float specStrength = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);

    specStrength = pow(max(dot(normal, halfwayDir), 0.0),64.0);
    vec3 specular = texture_specular * specStrength;
    
    //--------
    // SHADOWS
    //--------
    float bias = max(0.09 * (1.0 - dot(normal, lightDir)), 0.05);

    float shadow = caclualteShadow(fs_in.FragPosLight, bias);
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * texture(texture_diffuse0, texCoords).rgb;

    //-------------
    // FINAL RESULT
    //-------------    
    FragColor = vec4(result,1.0);
}