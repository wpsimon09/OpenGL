#version 330 core

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

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};


uniform PointLight pointLights[4];

out vec4 FragColor;

uniform sampler2D texture_diffuse0;
uniform sampler2D specular;

uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos,vec3 viewDir);


void main() 
{
    //----------
    // AMBIENT
    //----------
    vec3 ambient = texture(texture_diffuse0, fs_in.TexCoords).rgb * 0.2;
    
    //-----------
    // texture(texture_diffuse0, fs_in.TexCoords)
    //-----------
    vec3 normal = normalize(-fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diffStrength = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightColor * diffStrength * texture(texture_diffuse0, fs_in.TexCoords).rgb;
    
    //---------
    // SPECULAR
    //---------
    vec3 specularColor = lightColor;
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float specStrength = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    specStrength = pow(max(dot(normal, halfwayDir), 0.0),64.0);
    vec3 specular = specularColor * specStrength;

    vec3 result = ambient + diffuse + specular;
 

    //-------------
    // FINAL RESULT
    //-------------    
    FragColor = vec4(result,1.0);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos,vec3 viewDir)
{
	//----------
    // AMBIENT
    //----------
    vec3 ambient = texture(texture_diffuse0, fs_in.TexCoords).rgb * 0.2;
    
    //-----------
    // texture(texture_diffuse0, fs_in.TexCoords)
    //-----------
    vec3 normal = -normalize(normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diffStrength = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightColor * diffStrength * texture(texture_diffuse0, fs_in.TexCoords).rgb;
    
    //---------
    // SPECULAR
    //---------
    vec3 specularColor = lightColor;
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float specStrength = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    specStrength = pow(max(dot(normal, halfwayDir), 0.0),64.0);
    vec3 specular = specularColor * specStrength;

	
	diffuse *= attenuation;
	specular *= attenuation;
	vec3 emision = vec3(texture(material.texture_emmision0, TexCoord));
	return (ambient + diffuse + specular);
}