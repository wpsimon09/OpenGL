#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

//noraml vectors
in vec3 Normal;

//fragment positions;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

uniform sampler2D texture1;

void main(){

	//setting ratio because our light ray is comming throught the air (index 1.00)
	//to the glass (index 1.52) terefore we need to calculate the ratio of bend
	float ratio = 1.00/1.309;

	//calculate direction at which is player looking at the object
	vec3 I = normalize(Position - cameraPos);

	//caculate vector that will touch the skybox and we can retrieve its fragment color
	//this time using refract function that expects view vector, normal vectors and 
	//ratio between both materials
	vec3 R = refract(I, normalize(Normal), ratio);

	//grap the fragment color at which is r vector pointing at
	FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
