#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onPaintUI() override;

private:
  static int const m_N{15}; // Board size is m_N x m_N

  enum class GameState { Play, Draw, WinX, WinO };
  GameState m_gameState;

  bool m_XsTurn{true};
  int m_board[15][15]; // -1, 0 or 1

  ImFont *m_font{};

  void checkEndCondition(int i, int j);
  void restartGame();
};

#endif