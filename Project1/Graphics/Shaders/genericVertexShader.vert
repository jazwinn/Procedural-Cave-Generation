#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 uniform_m2w;

void main() {

    gl_Position = uniform_m2w * vec4(aPos, 1.0);
}