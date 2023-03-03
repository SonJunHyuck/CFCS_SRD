#version 400

// fragment shader : 색을 결정하는 단계

// fragment shader에서는 반드시 하나이상의 out이 있어야 한다.
// vec4 타입이어야 함

struct SpotLight
	{
		vec4 Position;
		vec3 Intensity;
		vec4 Direction;
		float Exponent;
		float Cut_Off;
		float Inner_Cut_Off;
	};

uniform SpotLight Spot;

// rasterization 후 각 vertex 의 색을 지정한다. (삼각형 내부 색은 보간)
in vec4 Position;
in vec3 Normal;
in vec3 Color;

uniform mat4 ViewMatrix;
uniform vec3 Ka;
uniform vec3 Kd; //오브젝트 색상
uniform vec3 Ks;

uniform float Shiness;

layout( location = 0 ) out vec4 FragColor;

void main()
{
	// Spot
	vec4 Spot_Position = ViewMatrix * Spot.Position;
	vec4 Spot_Direction = ViewMatrix * Spot.Direction;

	// step1. Init Attenuation
	float Attenuation = 0;
	float D = distance(Spot_Position, Position);
	Attenuation = 1 / ((1.0f) + (0.01f * D) + (0.001f * pow(D, 2)));

	// step2. Is in Inner_Cut_Off?
	vec4 spot_look = Spot_Direction - Spot_Position;
	vec4 spot_Inverse_L = Position - Spot_Position;
	float spot_look_distance = distance(Spot_Direction, Spot_Position);
	float spot_Inverse_L_distance = distance(Position, Spot_Position);
	float spot_dot = dot(spot_look, spot_Inverse_L);

	float betweenAngle = acos(spot_dot / (spot_look_distance * spot_Inverse_L_distance));
	float Spot_Attenuation = 1;

	if(radians(Spot.Inner_Cut_Off) > betweenAngle)
	{
		Spot_Attenuation = 1.0f;
	}
	else
	{
		float spot_Value = smoothstep(cos(radians(Spot.Cut_Off)), cos(radians(Spot.Inner_Cut_Off)), cos(betweenAngle));
		Spot_Attenuation = pow(spot_Value, Spot.Exponent);
	}

	Attenuation *= Spot_Attenuation;

   vec3 N = Normal;
   vec3 V = vec3(normalize(-Position));
   vec3 L = vec3(normalize(Spot_Position - Position));  //광원 벡터
   vec3 H = normalize(V + L);
   
   vec3 finalColor = vec3(0, 0, 0);

   vec3 ambient = Ka * (Spot.Intensity * Attenuation);
   vec3 diffuse = Kd * max(dot(L, N), 0) * (Spot.Intensity * Attenuation);
   vec3 specular = Ks * pow(max(dot(H, N), 0), Shiness) * (Spot.Intensity * Attenuation);
   
   finalColor =  ambient + diffuse + specular;

    FragColor = vec4(finalColor, 1.0f);
}