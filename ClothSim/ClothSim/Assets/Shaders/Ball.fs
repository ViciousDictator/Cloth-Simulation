#version 330 core

out vec4 fragment;

in vec3 Normal;
in vec3 FragPos;



void main(){

	vec3 LightColor = vec3(1,1,1);
	vec3 ObjectColor = vec3(0.3,0,0);

	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * LightColor;

	vec3 norm = normalize(Normal);
	vec3 LightDirection = normalize(vec3(0.5,0.5,0));

	float diff = max(dot(norm, LightDirection), 0.0);
	vec3 diffuse = diff * LightColor;

	//fragment = vec4(NewNormal,1.0f);
	vec3 result = (ambient + diffuse) * ObjectColor;
	fragment = vec4(result,1.0);
}