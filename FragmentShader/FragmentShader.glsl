#version 330

in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 fragmentColor;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float textureOpacity;

void main() {
	//ambient
	//------------
	float ambientStrengt = 0.7f;

	vec3 ambient = ambientStrengt * lightColor;

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
	vec3 diffuse = diff * lightColor;

	//specular
	//----------
	float specularStrength = 0.5f;
	
	//callculating view direction vector
	vec3 viewDirection = normalize(viewPos - FragPos);
	
	//calculating reflect direciton whiwch is reflectec light direciton vector around normal vector
	vec3 reflctDir = reflect(-lightDir, norm);

	//calculating specular strengt
	float spec = pow(max(dot(viewDirection, reflctDir), 0.0f), 64);
	
	//calculating color of specular lighting
	vec3 specular = specularStrength * spec * lightColor;

	vec3 resoult = (ambient + diffuse + specular) * objectColor;

	fragmentColor = vec4(resoult, 1.0);
}