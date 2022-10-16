#include "player.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Player::create(GLuint program) {
  destroy();

  m_program = program;

  // Get location of uniforms in the program
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Reset player attributes
  m_rotation = 0.0f;
  m_translation = glm::vec2(0);
  m_velocity = glm::vec2(0);

  // clang-format off
  std::array positions{
      // Ship body
      glm::vec2{-1.0f, +1.0f}, glm::vec2{-1.0f, -1.0f},
      glm::vec2{+1.0f, -1.0f}, glm::vec2{+1.0f, +1.0f}  
      };


  // Normalization
  for (auto &position : positions) {
    position /= glm::vec2{1.0f, 1.0f};
  }

  std::array const indices{0, 1, 3,
                           1, 2, 3};
  // clang-format on                           

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
  abcg::glUniform1f(m_rotationLoc, m_rotation);
  abcg::glUniform2fv(m_translationLoc, 1, &m_translation.x);

  abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);
  abcg::glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Player::destroy() {
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}


void Player::update(GameData const &gameData, float deltaTime) {
  
  // move right
  if (gameData.m_input[gsl::narrow<size_t>(Input::Right)]){
    xvel = +1.0f;
    m_translation = glm::vec2{m_translation.x + xvel * deltaTime,m_translation.y};
  }
  
  // move left
  if (gameData.m_input[gsl::narrow<size_t>(Input::Left)]){
    xvel = -1.0f;
    m_translation = glm::vec2{m_translation.x + xvel * deltaTime,m_translation.y};
  }

  // Wrap-around
  if (m_translation.x < -1.0f)
    m_translation.x = 1.0f;
  if (m_translation.x > +1.0f)
    m_translation.x = -1.0f;


  // hit bottom or platform 
  if (m_translation.y < -1.0f){
    m_translation.y = -1.0f;
    yvel = 0;
  }
    

  // player gravity-

  yacc = -2.0f;

  // jump
  // ADD JUMP COOLDOWN TO LIMIT JUMP HEIGHT
  
  if (yvel == 0){
    
    yvel = 1.5f;
  }
  // if not above platform

  yvel += yacc*deltaTime;

  m_translation.y += yvel*deltaTime;


  //stop falling if above platform



}