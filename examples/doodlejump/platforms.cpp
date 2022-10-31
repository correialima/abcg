#include "platforms.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Platforms::create(GLuint program,int quantity) {
  destroy();

  m_randomEngine.seed(
        std::chrono::steady_clock::now().time_since_epoch().count());
  m_program = program;

  // Get location of uniforms in the program
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Create platforms
  m_platforms.clear();
  m_platforms.resize(quantity);
  for (auto &platform : m_platforms) {
    platform = makePlatform();
  }
}

void Platforms::paint() {
  abcg::glUseProgram(m_program);

  for (auto const &platform : m_platforms) {
    abcg::glBindVertexArray(platform.m_VAO);

    abcg::glUniform4fv(m_colorLoc, 1, &platform.m_color.r);
    abcg::glUniform1f(m_scaleLoc, platform.m_scale);

    abcg::glUniform2f(m_translationLoc, platform.m_translation.x,
                      platform.m_translation.y);

    abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0,   4);


    abcg::glBindVertexArray(0);
  }

  abcg::glUseProgram(0);
}

void Platforms::destroy() {
  for (auto &platform : m_platforms) {
    abcg::glDeleteBuffers(1, &platform.m_VBO);
    abcg::glDeleteVertexArrays(1, &platform.m_VAO);
  }

  // reset inicial parameters for platforms
  step_height = -0.7f;
  horizontal_drift = 0.0f;
  m_velocity.y = -0.1f;
}



Platforms::Platform Platforms::makePlatform(){

  Platform platform;
  auto &re{m_randomEngine};

  // clang-format off
  platform.positions = {
      // pltatform body 
      glm::vec2{-1.0f, -0.2f}, //bottom left
      glm::vec2{-1.0f, +0.2f}, //top left
      glm::vec2{+1.0f, -0.2f}, //bottom right
      glm::vec2{+1.0f, +0.2f}  //top right
      };

  platform.m_translation.y = step_height;
  platform.m_translation.x = horizontal_drift;
  platform.height = step_height;

  // updates random variables for next platform
  horizontal_drift = m_randomDist(re);
  std::uniform_real_distribution randomVerticalDrift(-0.1f, +0.1f);
  step_height +=  +0.2f + randomVerticalDrift(re);
  
  platform.m_top_left = platform.positions[1]*platform.m_scale + platform.m_translation;
  platform.m_top_right = platform.positions[3]*platform.m_scale + platform.m_translation;

  // Generate VBO
  abcg::glGenBuffers(1, &platform.m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, platform.m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, platform.positions.size() * sizeof(glm::vec2),
                     platform.positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &platform.m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(platform.m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, platform.m_VBO);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0); 

  return platform;
}

void Platforms::update(const Player &player, float deltaTime) {

  
  for (auto &platform : m_platforms) {
    platform.m_top_left = platform.positions[1]*platform.m_scale + platform.m_translation;
    platform.m_top_right = platform.positions[3]*platform.m_scale + platform.m_translation; 
  }

  for(auto &platform : m_platforms){
    platform.m_translation.y += deltaTime * m_velocity.y;
  }

}