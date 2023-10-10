#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 3) in mat4 modelMatrix;
layout (location = 7) in vec4 ka;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 Color;

void main()
{	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(VertexPosition, 1);
	Color = ka;
}