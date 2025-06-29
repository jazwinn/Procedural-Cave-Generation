#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4x4 instancedMatrix;



uniform mat4 uniform_m2w;

void main() {
    gl_Position = uniform_m2w * instancedMatrix * vec4(aPos, 1.0);
}