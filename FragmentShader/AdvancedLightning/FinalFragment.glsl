#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D shadowMap;
uniform mat4 lightMatrix;

struct Light {
    vec3 Position;
    vec3 Color;
};

const int NR_LIGHTS = 1;
uniform Light light;
uniform vec3 viewPos;

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

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse =vec3(0.9, 0.9, 0.9) ;//texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = 0.5;//texture(gAlbedoSpec, TexCoords).a;
    vec4 FragPosLight = lightMatrix * vec4(FragPos, 1.0); //fragment postition

    // then calculate lighting as usual
    vec3 viewDir  = normalize(viewPos - FragPos);

    //final lightning color
    vec3 result;

    //--------
    // AMBIENT
    //--------
    vec3 ambient  = Diffuse * 0.1; // hard-coded ambient component
        
    //--------
    // DIFFUSE
    //--------
    vec3 lightDir = normalize(light.Position - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.Color;

    //---------
    // SPECULAR
    //---------
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
    vec3 specular = light.Color * spec * Specular;

    //------
    //SHADWO
    //------
    float bias = max(0.09 * (1.0 - dot(Normal, lightDir)), 0.05);

    float shadow = caclualteShadow(FragPosLight, bias);
    result += (ambient + (1.0 - shadow) * (diffuse + specular)) * Diffuse;
 
    //-------------------------
    // HDR AND GAMMA CORRECTION
    //-------------------------
    float exposure = 1.0;
    float gamma = 2.2;
    result = vec3(1.0) - exp(-result * exposure); 
    FragColor.rgb = pow(result, vec3(1/gamma));
    FragColor = vec4(result, 1.0);
}
