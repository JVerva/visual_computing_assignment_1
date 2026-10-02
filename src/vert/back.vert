#version 330 core
layout (location=0) in vec2 aTexCoords;

out vec2 texCoord;

void main(){
    texCoord = aTexCoords;
    gl_Position = vec4(aTexCoords * 2.0 - 1.0, 0.0, 1.0);
}
