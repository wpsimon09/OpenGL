#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 WorldPos;

uniform mat4  projection;
uniform mat4 view;
uniform mat4 model;
void main()
{
	WorldPos = aPos;
	
	mat4 rotView = mat4(mat3(view));
	vec4 clipPos = projection * rotView * vec4(WorldPos, 1.0);

	gl_Position = clipPos.xyww;
	//gl_Position = projection * view * model * vec4(aPos, 1.0);
}
