#version 330


//specifies the color of each ambient, diffuses, specular to simulat different materials
//shinines is for how big is the radius of specular hightlights
//-----------
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emmision;
	float shininess;
};

//specifies the intesity the light has on each of the properties
//in summary its the strength of the givven light component
//-----------
struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 fragmentColor;

//uniform sampler2D ourTexture;
//uniform sampler2D ourTexture2;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float textureOpacity;

uniform Material material;
uniform Light light;

void main() {

	//emmision
	//------------
	vec3 emmision = vec3(texture(material.emmision, TexCoord));

	//ambient
	//------------
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

	//diffuse
	//------------
	vec3 norm = normalize(Normal);

	// here we are calclating light direction vector to do it we have
	//to subtract light position from framgnet position
	//currently it is pointing from the fragment to the light possition
	vec3 lightDir = normalize(lightPos - FragPos);

	//calculating diffuse strength on each fragment
	float diff = max(dot(norm, lightDir), 0);

	//calculating diffuse strength for each fragment
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

	//specular
	//----------
	float specularStrength = 0.5f;
	
	//callculating view direction vector
	vec3 viewDirection = normalize(viewPos - FragPos);
	
	//calculating reflect direciton whiwch is reflectec light direciton vector around normal vector
	vec3 reflctDir = reflect(-lightDir, norm);

	//calculating specular strengt
	float spec = pow(max(dot(viewDirection, reflctDir), 0.0f), material.shininess);
	
	//calculating color of specular lighting (map)
	vec3 specular = light.specular * spec * vec3((texture(material.specular, TexCoord)));

	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	ambient *= attenuation;

	//resoult
	//------------
	vec3 resoult = ambient + diffuse + specular + emmision;
	fragmentColor = vec4(resoult, 1.0);
}