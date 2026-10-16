#version 330 core
layout (location=0) in vec2 a_uv;

out vec2 uv;

void main(){
    uv = a_uv;
    gl_Position = vec4(a_uv * 2.0 - 1.0, 0.0, 1.0);
}
