#version 330 core

layout (location = 0) in vec2 aPos;

	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
	gl_PointSize = 10.7;
}