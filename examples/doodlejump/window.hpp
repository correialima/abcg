#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <random>

#include "abcgOpenGL.hpp"

#include "player.hpp"
#include "platforms.hpp"
#include "clouds.hpp"


class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onUpdate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;
  void checkJump();
  void checkGameOver();
  void increaseSpeed();

private:
  glm::ivec2 m_viewportSize{};

  int score = {}, highScore = {};
  float speed{1.0f};
  GLuint m_objectsProgram{};

  GameData m_gameData;

  Player m_player;

  Platforms m_platforms;
  Clouds m_clouds;

  abcg::Timer m_restartWaitTimer;
  abcg::Timer speedUpCooldownTimer;

  ImFont *m_fontBig{}, *m_fontSmall{};

  std::default_random_engine m_randomEngine;

  void restart();
};

#endif