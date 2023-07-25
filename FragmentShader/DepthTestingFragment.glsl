#version 330 core

out vec4 FragColor;

//same name as in the vertex shader
in VS_OUT
{
	vec2 TexCoords;
} fs_in; //any name we want 

uniform sampler2D texture1;

void main(){
	if(gl_FrontFacing)
		FragColor = texture(texture1, fs_in.TexCoords);
	else
		//otherwise is a green color
		FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}