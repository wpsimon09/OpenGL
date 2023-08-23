#version 330 core

in vec4 FragPosition;

uniform vec3 lightPos;
uniform float far_Plane;

void main()
{
	// get the distance between light and the closest fragment
	float lightDistnace = length(FragPosition.xyz - lightPos);

	//put it to the range of [0,1]
	lightDistnace = lightDistnace/far_Plane;

	//output the calcualted depth value to the fragment's depth buffer
	gl_FragDepth = lightDistnace;
}