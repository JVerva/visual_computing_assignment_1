
#version 330 core
struct DirectionalLight{
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct Material{
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};

in vec3 f_position;
in vec3 f_normal;
in vec2 f_uv;

out vec4 f_color;

uniform DirectionalLight light;
uniform Material material;
uniform vec3 view_position;

void main()
{
    vec3 n_f_normal = normalize(f_normal);
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, f_uv));
    
    float diff = max(dot(-light.direction, n_f_normal), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, f_uv));
    
    vec3 view_direction = normalize(view_position - f_position);
    vec3 reflect_direction = reflect(light.direction, n_f_normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, f_uv));

    f_color = vec4(ambient + diffuse + specular, 1.0);
}




