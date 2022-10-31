#include "clouds.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Clouds::create(GLuint program,int quantity) {
  destroy();

  m_randomEngine.seed(
        std::chrono::steady_clock::now().time_since_epoch().count());
  m_program = program;

  // Get location of uniforms in the program
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Create clouds
  m_clouds.clear();
  m_clouds.resize(quantity);
  for (auto &cloud : m_clouds) {
    cloud = makeCloud();
  }
}

void Clouds::paint() {
  abcg::glUseProgram(m_program);

  for (auto const &cloud : m_clouds) {
    abcg::glBindVertexArray(cloud.m_VAO);

    abcg::glUniform4fv(m_colorLoc, 1, &cloud.m_color.r);
    abcg::glUniform1f(m_scaleLoc, cloud.m_scale);

    abcg::glUniform2f(m_translationLoc, cloud.m_translation.x,
                      cloud.m_translation.y);

    abcg::glDrawArrays(GL_TRIANGLE_FAN, 0,   22);
    abcg::glDrawArrays(GL_TRIANGLE_FAN, 22,   22);
    abcg::glDrawArrays(GL_TRIANGLE_FAN, 44,   22);
    abcg::glDrawArrays(GL_TRIANGLE_FAN, 66,   22);



    abcg::glBindVertexArray(0);
  }

  abcg::glUseProgram(0);
}

void Clouds::destroy() {
  for (auto &cloud : m_clouds) {
    abcg::glDeleteBuffers(1, &cloud.m_VBO);
    abcg::glDeleteVertexArrays(1, &cloud.m_VAO);
  }

  // reset inicial parameters for clouds
  step_height = -0.5f;
  horizontal_drift = 0.0f;
}



Clouds::Cloud Clouds::makeCloud(){

  Cloud cloud;
  auto &re{m_randomEngine};

  int sides{20};
  float radius{};
  std::vector<glm::vec2> positions;


    // first circle of cloud
    glm::vec2 center{};
    glm::vec2 pos{};
    radius = 0.2f;
  positions.emplace_back(center);

  // Border vertices
  auto step{M_PI * 2 / sides};
  for (auto const angle : iter::range(0.0, M_PI * 2, step)) {
    pos = {std::cos(angle)*radius, std::sin(angle)*radius};
    positions.emplace_back(center + pos);
  }

  // Duplicate second vertex
  positions.push_back(positions.at(1));

    // second circle of cloud
    center = {0.5f, 0};
  positions.emplace_back(center);

  // Border vertices
  step = M_PI * 2 / sides;
  radius = 0.3f;
  for (auto const angle : iter::range(0.0, M_PI * 2, step)) {

    pos = {std::cos(angle)*radius*1.5f, std::sin(angle)*radius};
    positions.emplace_back(center + pos);
  }

  // Duplicate second vertex
  positions.push_back(positions.at(23));

      // third circle of cloud
    center = {0.9f, 0.15f};
  positions.emplace_back(center);

  // Border vertices
  step = M_PI * 2 / sides;
  radius = 0.3f;
  for (auto const angle : iter::range(0.0, M_PI * 2, step)) {

    pos = {std::cos(angle)*radius, std::sin(angle)*radius};
    positions.emplace_back(center + pos);
  }

  // Duplicate second vertex
  positions.push_back(positions.at(45));

  // fourth circle of cloud
  center = {1.1f, -0.05f};
  positions.emplace_back(center);

  // Border vertices
  step = M_PI * 2 / sides;
  radius = 0.2f;
  for (auto const angle : iter::range(0.0, M_PI * 2, step)) {

    pos = {std::cos(angle)*radius*2.5f, std::sin(angle)*radius};
    positions.emplace_back(center + pos);
  }

  // Duplicate second vertex
  positions.push_back(positions.at(67));



  // updates random variables for next cloud
  horizontal_drift = m_randomDist(re) * 2.0f - 1.0f ;
  step_height +=  (+0.2f + (m_randomDist(re) * 0.2f - 0.1f));


  cloud.m_translation.y = step_height;
  cloud.m_translation.x = horizontal_drift;
  cloud.height = step_height;



  cloud.m_color[3] = m_randomDist(re) * 0.3f + 0.6f;

  
  // Generate VBO
  abcg::glGenBuffers(1, &cloud.m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, cloud.m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &cloud.m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(cloud.m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, cloud.m_VBO);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0); 

  return cloud;
}

void Clouds::update( float deltaTime) {

  for(auto &cloud : m_clouds){
    cloud.m_translation.y += deltaTime * m_velocity.y;
  }

}