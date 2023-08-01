#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
	vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.4;

uniform mat4 projection;

// draws a normal vector for the given vertex
void GenerateLine(int index) 
{
	// create starting point for a normal vector
	gl_Position = projection * gl_in[index].gl_Position;
	EmitVertex();
	
	//craete ending point for a normal vector
	gl_Position = projection * (gl_in[index].gl_Position +
	vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
	EmitVertex();
	
	// put them together and output them as a line
	EndPrimitive();
}

void main() {

	//draw normal vector for the given vertex
	GenerateLine(0);
	GenerateLine(1);
	GenerateLine(2);
}