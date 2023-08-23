#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPosition;

void main()
{
	// for each face 
	for(int face = 0; face < 6; ++face)
	{
		gl_Layer = face;
		//for each vertex of the triangle
		for(int triangleVertex = 0; triangleVertex < 3; ++triangleVertex)
		{
			//get the vertex position from the vertex shader
			FragPosition = gl_in[triangleVertex].gl_Position;
			// transform to the light's POV
			gl_Position = shadowMatrices[face] * FragPosition;
			//draw the vertex to the face specified in the face variable
			EmitVertex();
		}
		//draw the triagnle
		EndPrimitive();
	}
}