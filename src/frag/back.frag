#version 330 core
out vec4 f_color;

in vec2 uv;
uniform sampler2D background;

void main()
{
  f_color = texture(background, uv);
}
