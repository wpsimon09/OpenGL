#version 330 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLight;
}fs_in;

struct PointLight {
	float constant;
	float linear;
	float quadratic;
	
	vec3 position;

    vec3 color;
};

struct DirectionLight{
    vec3 position;
    vec3 color;
};


uniform PointLight pointLights[4];

uniform DirectionLight directionLight;

out vec4 FragColor;

uniform sampler2D texture_diffuse0;

uniform sampler2D shadowMap;

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

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos,vec3 viewDir)
{

	// here we are calclating light direction vector to do it we have
	//to subtract light position from framgnet position
	//currently it is pointing from the fragment to the light possition
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading

	float diff = max(dot(normal, lightDir), 0.0);
	
	// specular shading
     vec3 halfwayDir = normalize(lightDir + viewDir);

	float spec = pow(max(dot(normal, halfwayDir), 0.0),
	8);
	
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	// combine results
	vec3 ambient = texture(texture_diffuse0,fs_in.TexCoords).rgb * 0.3;
	vec3 diffuse = light.color * diff * texture(texture_diffuse0,fs_in.TexCoords).rgb;
	vec3 specular = light.color * spec * texture(texture_diffuse0,fs_in.TexCoords).rgb;
	
	diffuse *= attenuation;
	specular *= attenuation;

    float bias = max(0.09 * (1.0 - dot(normal, lightDir)), 0.05);

    float shadow = caclualteShadow(fs_in.FragPosLight, bias);
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * texture(texture_diffuse0, fs_in.TexCoords).rgb;


	return result;
}


vec3 CalcDirectionalLight(DirectionLight light, vec3 normal, vec3 fragPos,vec3 viewDir)
{

	// here we are calclating light direction vector to do it we have
	//to subtract light position from framgnet position
	//currently it is pointing from the fragment to the light possition
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading

	float diff = max(dot(normal, lightDir), 0.0);
	
	// specular shading
     vec3 halfwayDir = normalize(lightDir + viewDir);

	float spec = pow(max(dot(normal, halfwayDir), 0.0),
	8);
	
	// combine results
	vec3 ambient = texture(texture_diffuse0,fs_in.TexCoords).rgb * 0.3;
	vec3 diffuse = light.color * diff * texture(texture_diffuse0,fs_in.TexCoords).rgb;
	vec3 specular = light.color * spec * texture(texture_diffuse0,fs_in.TexCoords).rgb;
	
    float bias = max(0.09 * (1.0 - dot(normal, lightDir)), 0.05);

    float shadow = caclualteShadow(fs_in.FragPosLight, bias);
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * texture(texture_diffuse0, fs_in.TexCoords).rgb;


	return result;
}

void main() 
{

    vec3 norm = normalize(fs_in.Normal);
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    vec3 result = vec3(0.0);

    for(int i = 0; i<4; i++)
    {
        result += CalcPointLight(pointLights[i], norm,fs_in.FragPos, viewDir);
    }

    result += CalcDirectionalLight(directionLight, norm, fs_in.FragPos, viewDir);

    //-------------
    // FINAL RESULT
    //-------------    
    FragColor = vec4(result,1.0);
}


