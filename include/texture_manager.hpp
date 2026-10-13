#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include "stb_image.h"
#include "utils.hpp"
#include <cstdio>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <map>
#include <string>

class TextureManager {
public:
  bool load_texture_from_file(const std::string &path, GLuint &id) {
    if (file_loaded_textures.contains(path)) {
      id = file_loaded_textures[path];
      return true;
    }
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    // set the texture wrapping/filtering options (on currently bound texture)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, channels;
    unsigned char *data =
        stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    } else {
      fprintf(stderr, "Failed to load texture\n");
      return false;
    }
    file_loaded_textures[path] = id;
    return true;
  }

  void load_texture_from_color(unsigned int color, GLuint &id) {
    if (color_loaded_textures.contains(color)) {
      id = color_loaded_textures[color];
      return;
    }
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    // set the texture wrapping/filtering options (on currently bound texture)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    unsigned char data[] = {i_red_channel(color), i_green_channel(color),
                            i_blue_channel(color)};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 data);
    glGenerateMipmap(GL_TEXTURE_2D);
    color_loaded_textures[color] = id;
  }

private:
  std::map<std::string, GLuint> file_loaded_textures;
  std::map<unsigned int, GLuint> color_loaded_textures;
};

#endif
