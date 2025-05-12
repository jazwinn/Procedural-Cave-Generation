#version 330 core

out vec4 FragColor;

in float yColor;

void main(){

	float normalizedY = clamp(yColor, 0.0, 1.0);
	FragColor = vec4(vec3(normalizedY), 1.0);

}