#version 330 core


out vec4 FragColor;
in float aVisable;

uniform vec4 uniform_color;

void main(){
    if(aVisable < 0.5f) {
        discard;
    }
	FragColor = uniform_color;

}