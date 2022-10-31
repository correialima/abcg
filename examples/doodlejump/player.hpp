#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include "abcgOpenGL.hpp"

#include "gamedata.hpp"

class Player {
public:
  void create(GLuint program);
  void paint(GameData const &gameData);
  void destroy();
  void update(GameData const &gameData, float deltaTime);

  // graphic attributes
  glm::vec4 m_color{0.25f,0.0f,0.50f,1.0f};
  float m_scale{0.075f};

  // movement attributes
  std::array<glm::vec2,12> positions;
  glm::vec2 m_translation{};
  glm::vec2 m_velocity{};
  glm::vec2 m_acceleration{};
  bool jump{false};

  abcg::Timer m_jumpCoolDownTimer;

  // hitbox attributes
  glm::vec2 m_bottom_left{};
  glm::vec2 m_bottom_right{}; 

private:
  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
};
#endif