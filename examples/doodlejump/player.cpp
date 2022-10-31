#include "player.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Player::create(GLuint program) {
  destroy();

  m_program = program;

  // Get location of uniforms in the program
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Reset player attributes
  m_translation = glm::vec2(0);
  m_velocity = glm::vec2(0);

  // clang-format off
  positions = {
      // Doodle body
      glm::vec2{-8.0f, -10.0f}, //bottom left
      glm::vec2{-8.0f, +02.0f},
      glm::vec2{-04.0f, +04.0f},
      glm::vec2{-2.0f,  +10.0f},
      glm::vec2{+2.0f,  +10.0f},
      glm::vec2{+04.0f, +04.0f},
      glm::vec2{+8.0f, +02.0f},
      glm::vec2{+8.0f, -10.0f}, //bottom right
      glm::vec2{+4.0f, -10.0f},
      glm::vec2{+3.0f, -05.0f},
      glm::vec2{-3.0f, -05.0f},
      glm::vec2{-4.0f, -10.0f}};

  // Normalize
  for (auto &position : positions) {
    position /= glm::vec2{10.0f, 10.0f};
  }

  // creates reference points for player "hitbox", used for detecting jumps
  m_bottom_left = positions[0]*m_scale + m_translation;
  m_bottom_right = positions[7]*m_scale + m_translation;

  std::array const indices{ 0, 1, 11,
                            1, 10, 11,
                            1, 2, 10,
                            2, 5, 10,
                            2, 3,  4,
                            2, 4,  5,
                            5, 9, 10,
                            5, 9, 6,
                            6, 9, 8,
                            6, 7, 8
                            };

  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Player::paint(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;

  abcg::glUseProgram(m_program);

  abcg::glBindVertexArray(m_VAO);

  abcg::glUniform1f(m_scaleLoc, m_scale);
  abcg::glUniform2fv(m_translationLoc, 1, &m_translation.x);

  abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);
  abcg::glDrawElements(GL_TRIANGLES, 10 * 3, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Player::destroy() {
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
  m_velocity.y = 0.0f;
}


void Player::update(GameData const &gameData, float deltaTime) {
  
  // updating player hitbox to handle jumping
  m_bottom_left = positions[0]*m_scale + m_translation;
  m_bottom_right = positions[7]*m_scale + m_translation;

  // move right
  if (gameData.m_input[gsl::narrow<size_t>(Input::Right)]){
    m_velocity.x = +1.0f;
    m_translation = glm::vec2{m_translation.x + m_velocity.x * deltaTime,m_translation.y};
  }
  
  // move left
  if (gameData.m_input[gsl::narrow<size_t>(Input::Left)]){
    m_velocity.x = -1.0f;
    m_translation = glm::vec2{m_translation.x + m_velocity.x * deltaTime,m_translation.y};
  }

  // Wrap-around horizontally
  if (m_translation.x < -1.0f)
    m_translation.x = 1.0f;
  if (m_translation.x > +1.0f)
    m_translation.x = -1.0f;

  // jump
  if (jump){
    m_velocity.y = 1.0f;
    jump = false;
  }

  // player gravity
  m_acceleration.y = -1.0f;
  m_velocity.y += m_acceleration.y * deltaTime;
  m_translation.y += m_velocity.y * deltaTime;

}