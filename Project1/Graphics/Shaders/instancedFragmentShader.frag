#version 330 core

in vec2 vNormal;

out vec4 FragColor;


uniform vec4 uniform_color;

void main(){
	// Set the fragment color to the uniform colo
	FragColor = uniform_color;


}