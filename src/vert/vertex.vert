#version 330 core
layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 uv;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 f_position;
out vec3 f_normal;
out vec2 f_uv;

void main(){
    gl_Position = projection * view * model * vec4(position, 1.0);
    f_position = vec3(model * vec4(position, 1.0));
    f_normal = mat3(transpose(inverse(model))) * normal;
    f_uv = uv;
}
