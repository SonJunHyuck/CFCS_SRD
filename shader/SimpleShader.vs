#version 330 core

in vec4 VertexPosition;
in vec3 VertexNormal;
in vec3 VertexColor;

uniform mat4 ViewMatrix;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

out vec4 Position;
out vec3 Normal;
out vec3 Color;

void main()
{
	Position = ModelViewMatrix * VertexPosition;
	Normal = normalize(NormalMatrix * VertexNormal);
	Color = VertexColor;

	gl_Position = MVP * VertexPosition;
}