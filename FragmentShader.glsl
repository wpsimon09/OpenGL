#version 330

in vec3 ourColor;
out vec4 fragmentColor;
void main() {
	fragmentColor = vec4(ourColor, 1.0f);
}