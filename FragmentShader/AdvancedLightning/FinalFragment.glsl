#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorAndShinines;

const int NR_OF_LIGHTS = 32;

struct Light{
	vec3 position;
	vec3 color;
};

uniform Light lights[NR_OF_LIGHTS];
uniform vec3 viewPos;

vec3 CalcLight(Light light, vec3 normal, vec3 fragPos,vec3 viewDir);

vec3 FragPos;
vec3 Normal;
vec3 texture_diffuse;
float texture_specular;

void main()
{
	vec3 result;
	vec3 FragPos = texture(gPosition, TexCoord).rgb;
	vec3 Normal = texture(gNormal, TexCoord).rgb;
	vec3 texture_diffuse = texture(gColorAndShinines, TexCoord).rgb;
	float texture_specular = texture(gColorAndShinines, TexCoord).a;

	vec3 viewDir = normalize(viewPos - FragPos);

	for(int i = 0; i< NR_OF_LIGHTS; i++)
	{
		result += CalcLight(lights[i], Normal, FragPos, viewDir);
	}
	FragColor = vec4(texture_diffuse,1.0);
}

vec3 CalcLight(Light light, vec3 normal, vec3 fragPos,vec3 viewDir)
{

	vec3 lightDir = normalize(light.position - fragPos);
	
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	
	// specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);

	float spec = pow(max(dot(normal, halfwayDir), 0.0),
	8);
	
	// combine results
	vec3 ambient = texture_diffuse * 0.6;
	vec3 diffuse = light.color * diff * texture_diffuse;
	vec3 specular = light.color * spec * texture_specular;
	
	vec3 result = ambient + diffuse + specular;
    
	return result;

}