#version 330 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
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

uniform sampler2D texture_diffuse0;
uniform sampler2D specular;
uniform sampler2D lightSource;

uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;


vec3 CalcDirectionalLight(DirectionLight light, vec3 normal, vec3 fragPos,vec3 viewDir);

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos,vec3 viewDir);


void main() 
{
	vec3 norm = normalize(fs_in.Normal);
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);

	vec3 result = vec3(0.0);
	//Directional lighting
	for(int i = 0; i<4; ++i)
	{
		result += CalcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir);
	}

	result += CalcDirectionalLight(directionLight, norm,fs_in.FragPos, viewDir);

    //-------------
    // FINAL RESULT
    //-------------
	FragColor = vec4(result, 1.0);

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
		if(brightness > 1.0)
			BrightColor = vec4(FragColor.rgb, 1.0);
		else
			BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
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
	vec3 ambient = texture(texture_diffuse0,fs_in.TexCoords).rgb * 0.0;
	vec3 diffuse = light.color * diff * texture(texture_diffuse0,fs_in.TexCoords).rgb;
	vec3 specular = light.color * spec * texture(texture_diffuse0,fs_in.TexCoords).rgb;
	
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}

vec3 CalcDirectionalLight(DirectionLight light, vec3 normal, vec3 fragPos,vec3 viewDir)
{

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
	
	vec3 result = ambient + diffuse + specular;
    
	return result;
}