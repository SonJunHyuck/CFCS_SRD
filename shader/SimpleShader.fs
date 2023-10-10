#version 330 core

in vec4 Position;
in vec3 Normal;
in vec3 Color;

uniform mat4 ViewMatrix;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

uniform vec4 LightPosition;
uniform vec3 LightIntensity;

uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;

uniform vec3 CamPos;
uniform float Shiness;

layout( location = 0 ) out vec4 FragColor;

void main()
{
	mat4 iv = inverse(ModelViewMatrix);
	mat3 NormalMatrix = mat3(transpose(iv));

	vec4 P = ModelViewMatrix * Position;
	vec3 L = normalize(ViewMatrix * LightPosition - P).xyz;
	vec3 N = normalize(NormalMatrix * Normal);
	vec3 V = normalize(-P.xyz);
	vec3 R = reflect(-L, N);

	vec3 ambient = LightIntensity * Ka;
	vec3 diffuse = LightIntensity * Kd * max(dot(L, N), 0);
	vec3 specular = LightIntensity * Ks * pow(max(dot(R, V), 0.0), Shiness);

	FragColor = vec4(Color + ambient + diffuse + specular, 1.0);

}
