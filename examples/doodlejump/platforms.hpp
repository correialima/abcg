#ifndef PLATFORMS_HPP_
#define PLATFORMS_HPP_

#include "abcgOpenGL.hpp"

#include <list>
#include <random>

#include "gamedata.hpp"
#include "player.hpp"

class Platforms {
public:
  void create(GLuint program,int quantity);
  void paint();
  void destroy();
  void update(const Player &player, float deltaTime);


  struct Platform {
    GLuint m_VAO{};
    GLuint m_VBO{};
    glm::vec4 m_color{0.329f,0.208f,0.082f,1.0f};
    float m_rotation{};
    float m_scale{0.10f};
    glm::vec2 m_translation{};
    glm::vec2 m_velocity{};

  };

  std::list<Platform> m_platforms;

  Platform makePlatform(glm::vec2 translation = {}, float scale = 0.10f); 

private:
  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};
  GLint m_rotationLoc{};

  std::default_random_engine m_randomEngine;
  std::uniform_real_distribution<float> m_randomDist{-0.8f, 0.8f};
};
#endif