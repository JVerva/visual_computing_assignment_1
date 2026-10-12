
#version 330 core
layout(location = 0) in vec3 f_normal;
layout(location = 1) in vec2 f_uv;

out vec4 f_color;

uniform sampler2D f_texture;

void main()
{
    f_color = texture(f_texture, f_uv);
}




