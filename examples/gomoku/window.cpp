#include "window.hpp"
#include <iostream>
using namespace std;

void Window::onCreate() {

  restartGame();
}

void Window::onPaintUI() {
  // Get size of application's window
  auto const appWindowWidth{gsl::narrow<float>(getWindowSettings().width)};
  auto const appWindowHeight{gsl::narrow<float>(getWindowSettings().height)};

  // "Gomoku" window
  {
    ImGui::SetNextWindowSize(ImVec2(appWindowWidth, appWindowHeight));
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    auto const flags{ ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar};
    ImGui::Begin("Gomoku", nullptr, flags);

    // Static text showing current turn or win/draw messages
    {
      std::string text;
      switch (m_gameState) {
      case GameState::Play:
        text = fmt::format("{}'s turn", m_XsTurn ? 'X' : 'O');
        break;
      case GameState::Draw:
        text = "Draw!";
        break;
      case GameState::WinX:
        text = "X's player wins!";
        break;
      case GameState::WinO:
        text = "O's player wins!";
        break;
      }
      // Center text
      ImGui::SetCursorPosX(
          (appWindowWidth - ImGui::CalcTextSize(text.c_str()).x) / 2);
      ImGui::Text("%s", text.c_str());
      ImGui::Spacing();
    }

    ImGui::Spacing();
    
        // Game board
    {
      auto const gridHeight{appWindowHeight - 22 - (m_N * 10) - 60};
      auto const buttonHeight{gridHeight / m_N};

      // Use custom font
      if (ImGui::BeginTable("Game board", m_N)) {
        for (auto i : iter::range(m_N)) {
          ImGui::TableNextRow();
          for (auto j : iter::range(m_N)) {
            ImGui::TableSetColumnIndex(j);
            // Get current character

            auto buttonChar{' '};
            switch (m_board[i][j]) {
            case 0:
              buttonChar = ' ';
              break;
            case 1:
              buttonChar = 'X';
              break;
            case -1:
              buttonChar = 'O';
              break;
            }
            auto buttonText = fmt::format("{}##{:#02}{:#02}", buttonChar, i, j);
            
            if (ImGui::Button(buttonText.c_str(), ImVec2(-1, buttonHeight))) {
              cout << m_board[i][j];
              cout << " - ";
              cout << buttonText << endl;
              if (m_gameState == GameState::Play && m_board[i][j] == 0) {
                m_board[i][j] = m_XsTurn ? 1 : -1;
                checkEndCondition(i,j);
                m_XsTurn = !m_XsTurn;
              }
            }
          }
          ImGui::Spacing();
        }
        ImGui::EndTable();
      }
    }

    ImGui::Spacing();

        // "Restart game" button
    {
      if (ImGui::Button("Restart game", ImVec2(-1, 50))) {
        restartGame();
      }
    }

    ImGui::End();
  }
}

void Window::restartGame() {
  for(auto i : iter::range(m_N)) {
    for(auto j : iter::range(m_N)) {
      m_board[i][j] = 0;
    }
  }
  m_XsTurn = true;
  m_gameState = GameState::Play;
}

void Window::checkEndCondition(int i, int j) {

  auto minx{0}, maxx{0}, miny{0}, maxy{0}, sum_hor{0}, sum_ver{0}, sum_d1{0}, sum_d2{0};

  minx = max(j-4,0);
  miny = max(i-4,0);
  
  maxx = min(j+4,15);
  maxy = min(i+4,15);
  
  //check horizontal
  for(auto k : iter::range(minx, maxx)) {
    sum_hor += m_board[i][k];
  }

  //check vertical
  for(auto k : iter::range(miny, maxy)) {
    sum_ver += m_board[k][j];
  }

  //check first diagonal
  for(auto k : iter::range(miny, maxy)) {
    sum_d1 += m_board[i][k];
  }

  //check second diagonal
  for(auto k : iter::range(miny, maxy)) {
    sum_d2 += m_board[i][k];
  }
  
  if (sum_hor == 5 || sum_ver == 5 || sum_d1 == 5 || sum_d2 == 5) {
    m_gameState = GameState::WinX;
  }else if (sum_hor == -5 || sum_ver == -5 || sum_d1 == -5 || sum_d2 == -5) {
    m_gameState = GameState::WinO;
  }

}
