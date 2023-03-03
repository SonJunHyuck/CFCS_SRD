#version 430 core
layout (location = 0) in vec3 aPos;

out vec4 vertexColor;

uniform mat4 mvp;

void main ()
{
	gl_Position = mvp * vec4(aPos, 1.0f);
	//gl_Position = vec4(aPos, 1.0f);
	vertexColor = vec4(0.5, 0.0, 0.0, 1.0);
}