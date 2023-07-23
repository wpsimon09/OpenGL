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
	//calculate direction at which is player looking at the object
	vec3 I = normalize(Position - cameraPos);

	//caculate vector that will touch the skybox and we can retrieve its fragment color
	vec3 R = reflect(I, normalize(Normal));

	//grap the fragment color at which is r vector pointing at
	FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
