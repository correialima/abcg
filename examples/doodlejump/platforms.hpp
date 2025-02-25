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

  // struct for a single platform 
  struct Platform {
    GLuint m_VAO{};
    GLuint m_VBO{};

    // graphics attributes
    glm::vec4 m_color{0.329f,0.208f,0.082f,1.0f};
    float m_scale{0.10f};
    std::array<glm::vec2,4> positions;

    // movement attributes
    glm::vec2 m_translation{};

    float height{}; // used for score calculation

    // hitbox attributes
    glm::vec2 m_top_left{};
    glm::vec2 m_top_right{};
    

  };


  glm::vec2 m_velocity{-0.1f,0.0f}; // movement speed for all platforms
  
  std::list<Platform> m_platforms;

  Platform makePlatform(); 

private:
  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};


  // random attributes for positioning platformsl
  float step_height{-0.7f};
  float horizontal_drift{0.0f};
  std::default_random_engine m_randomEngine;
  std::uniform_real_distribution<float> m_randomDist{-0.8f, 0.8f};

};
#endif