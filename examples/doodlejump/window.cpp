#include "window.hpp"

void Window::onEvent(SDL_Event const &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Right));
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Right));
  }
}


void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  // Load fonts in two sizes, one for score information and one for game over message
  auto const filename{assetsPath + "ka1.ttf"};
  m_fontBig = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 50.0f);
  m_fontSmall = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 25.0f);
  if (m_fontBig == nullptr) {
    throw abcg::RuntimeError("Cannot load font file");
  }

  // Create program to render the other objects
  m_objectsProgram =
      abcg::createOpenGLProgram({{.source = assetsPath + "objects.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "objects.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  abcg::glClearColor(0.529f, 0.808f, 0.922f, 1);
  abcg::glEnable(GL_BLEND);
  abcg::glBlendEquation(GL_FUNC_ADD);
  abcg::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 


#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  restart();
}


void Window::restart() {
  m_gameData.m_state = State::Playing;

  m_player.create(m_objectsProgram);
  m_platforms.create(m_objectsProgram,1000);
  m_clouds.create(m_objectsProgram,500);

  speed = 1.0f;
  score = 0; // reseting score for next try

}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  // Wait 5 seconds before restarting
  if (m_gameData.m_state != State::Playing &&
      m_restartWaitTimer.elapsed() > 3) {
    restart();
    return;
  }

  // updating player and platforms
  m_player.update(m_gameData, deltaTime);
  m_platforms.update(m_player,deltaTime);
  m_clouds.update(deltaTime);
  
  // checks for player jumps on platforms and for game over condition
  if (m_gameData.m_state == State::Playing) {
    checkJump();
    checkGameOver();
    increaseSpeed();
  }
}


void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  m_player.paint(m_gameData);
  m_platforms.paint();
  m_clouds.paint();
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  {
    // flags for score and game over windows
    ImGuiWindowFlags const flags{ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_NoTitleBar |
                                 ImGuiWindowFlags_NoInputs};
                                 
    // creating texts for the score information window
    auto const scoreString = fmt::format("Score: {}",score);
    const char *scoreString_ = scoreString.c_str();

    auto const highScoreString = fmt::format("High Score: {}",highScore);
    const char *highScoreString_ = highScoreString.c_str();


    auto const speedString = fmt::format("Speed {:.1f}x",speed);
    const char *speedString_ = speedString.c_str();


    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,0,0,255));

    ImGui::SetNextWindowSize(ImVec2(600,50));
    ImGui::SetNextWindowPos({ImVec2(0.0f,m_viewportSize.y - 40)});

    ImGui::Begin("Score", nullptr, flags);
    ImGui::PushFont(m_fontSmall);

    ImGui::Text(scoreString_);
    ImGui::SameLine();
    ImGui::Text(highScoreString_);
    ImGui::PopFont();

    ImGui::End();

    ImGui::SetNextWindowSize(ImVec2(600,50));
    ImGui::SetNextWindowPos({ImVec2(0,m_viewportSize.y - 70)});

    ImGui::Begin("Speed", nullptr, flags);
    ImGui::PushFont(m_fontSmall);

    ImGui::Text(speedString_);
    ImGui::PopFont();

    ImGui::End();
    

    // creating game over window, shown if game is over
    auto const size{ImVec2(600, 85)};
    auto const position{ImVec2((m_viewportSize.x - size.x) / 2.0f,
                               (m_viewportSize.y - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);

    ImGui::Begin("Game Over", nullptr, flags);
    ImGui::PushFont(m_fontBig);

    ImGui::SetCursorPosX(
        (m_viewportSize.y - ImGui::CalcTextSize("Game Over!").x) / 2);
    if (m_gameData.m_state == State::GameOver) {
      ImGui::Text("Game Over!");
    }

    ImGui::PopFont();
    ImGui::PopStyleColor();
    ImGui::End();
  }
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void Window::onDestroy() {
  abcg::glDeleteProgram(m_objectsProgram);

  m_player.destroy();
  m_platforms.destroy();
  m_clouds.destroy();
}

void Window::checkJump() {
  // checks if player has jumped on any of the platforms

  for(auto const &platform : m_platforms.m_platforms){
    if (abs(m_player.m_bottom_left.y - platform.m_top_left.y) < 0.01f){ // if the bottom of player is level with the platform top
      //fmt::print("{} {} {}\n", m_player.m_bottom_left.x, platform.m_top_left.x, platform.m_top_right.x);
      if((m_player.m_bottom_left.x < platform.m_top_right.x && m_player.m_bottom_left.x > platform.m_top_left.x) ||   // if player left side is inside the platform
          (m_player.m_bottom_right.x > platform.m_top_left.x && m_player.m_bottom_right.x < platform.m_top_right.x)){ // or if right side of player is inside platform
            
        if(m_player.m_velocity.y < 0){ // if player is falling
          m_player.jump = true;
          
          score = std::max(score,(int) ((platform.height + 0.7f) *10.0f * speed)); // updates score
          if (score >= highScore){ 
            highScore = score; // updates highscore if score is above it
          }
        }
      }
    }
  }
}



void Window::checkGameOver() {
  // check if player hits the bottom of the screen

  if(m_player.m_bottom_left.y < -1.0f){
    m_gameData.m_state = State::GameOver; // updates game state
    m_restartWaitTimer.restart();
  }
}

void Window::increaseSpeed() {
  // check if player hits the bottom of the screen

  if (m_player.m_translation.y > 1.0f && speedUpCooldownTimer.elapsed() > 10){
    speed *= 1.2f;
    m_platforms.m_velocity.y *= 1.2f;
    speedUpCooldownTimer.restart();
  }
}