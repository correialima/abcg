#ifndef CLOUDS_HPP_
#define CLOUDS_HPP_

#include "abcgOpenGL.hpp"

#include <list>
#include <random>

#include "gamedata.hpp"
#include "player.hpp"

class Clouds {
public:
  void create(GLuint program,int quantity);
  void paint();
  void destroy();
  void update(float deltaTime);

  // struct for a single Cloud 
  struct Cloud {
    GLuint m_VAO{};
    GLuint m_VBO{};

    // graphics attributes
    glm::vec4 m_color{1.0f,1.0f,1.0f,0.5f};
    float m_scale{0.35f};

    // movement attributes
    glm::vec2 m_translation{};
  };


  glm::vec2 m_velocity{0.0f,-0.1f}; // movement speed for all clouds
  
  std::list<Cloud> m_clouds;

  Cloud makeCloud(); 

private:
  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};


  // random attributes for positioning platformsl
  float step_height{-0.7f};
  float horizontal_drift{0.0f};
  std::default_random_engine m_randomEngine;
  std::uniform_real_distribution<float> m_randomDist{0.0f, 1.0f};

};
#endif