#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 fColor;

in VS_OUT {
	vec2 texCoords;
} gs_in[];

out vec2 TexCoords;

uniform float time;

vec3 GetNormal()
{
	//calculate 2 paraler vectors to the triangle
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);

	// calculate the normal vector
	// we need to do the cross product between A and B not other way aroun 
	// otherwise the vector would be pointing to another direction

	return normalize(cross(a, b));
}


vec4 explode(vec4 position, vec3 normal)
{
	float magnitude = 2.0;

	// calculate where the triangles should move 
	vec3 direction = normal * ((sin(time)+1.0)/2.0) * magnitude;

	// move the current position to the new one 
	return position + vec4(direction ,0.0);
}

void build_house(vec4 position)
{
	gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0); // 1:bottom-left
	EmitVertex();
	
	gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0); // 2:bottom-right
	EmitVertex();
	
	gl_Position = position + vec4(-0.2, 0.2, 0.0, 0.0); // 3:top-left
	EmitVertex();
	
	gl_Position = position + vec4( 0.2, 0.2, 0.0, 0.0); // 4:top-right
	EmitVertex();
	
	gl_Position = position + vec4( 0.0, 0.4, 0.0, 0.0); // 5:top
	fColor = vec3(1.0, 1.0, 1.0);
	EmitVertex();
	EndPrimitive();
}
void main() {
	vec3 normal = GetNormal();

	// 1st vertex
	gl_Position = explode(gl_in[0].gl_Position, normal);
	TexCoords = gs_in[0].texCoords;
	EmitVertex();

	// 2nd vertex
	gl_Position = explode(gl_in[1].gl_Position, normal);
	TexCoords = gs_in[1].texCoords;
	EmitVertex();

	// 3rd vertex
	gl_Position = explode(gl_in[2].gl_Position, normal);
	TexCoords = gs_in[2].texCoords;
	EmitVertex();

	EndPrimitive();
}