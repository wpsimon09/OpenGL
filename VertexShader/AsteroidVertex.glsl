#version 330 

layout (location = 0) in vec3 aPosAsteroids;
layout (location = 2) in vec2 aTexCoordsAsteroids;
layout (location = 3) in mat4 instanceMatrix;

out vec2 TexCoordsAsteroids;

uniform mat4 view;
uniform mat4 projection;

void main() {

	gl_Position = projection * view * instanceMatrix * vec4(aPosAsteroids, 1.0);

	TexCoordsAsteroids = aTexCoordsAsteroids;
}