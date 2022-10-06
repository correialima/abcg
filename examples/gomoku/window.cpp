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
            
            
            auto buttonText = fmt::format("##{:#02}{:#02}", i, j);
            
            ImGui::PushID(i*100+j);

            if (m_board[i][j] == 0){
              ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.0f, 0.48f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.0f, 0.48f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.0f, 0.48f));
            }else if (m_board[i][j] == 1){
              ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 1.0f, 1.0f));
            }else if (m_board[i][j] == -1){
              ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.66f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.66f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.66f, 1.0f, 1.0f));
            }
            if (ImGui::Button(buttonText.c_str(), ImVec2(-1, buttonHeight))) {
              if (m_gameState == GameState::Play && m_board[i][j] == 0) {
                m_board[i][j] = m_XsTurn ? 1 : -1;
                checkEndCondition(i,j);
                m_XsTurn = !m_XsTurn;
              }
            }

            ImGui::PopStyleColor(3);
            ImGui::PopID();
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

  auto num_pos{0}, num_neg{0}, a{0}, b{0};

  //check horizontal
  for(auto k : iter::range(-4, 5)) {
    a = j + k;
    if (a >= 0 && a <= m_N + 1){
      if (num_pos == 5 || num_neg == 5) {break;}
      if(m_board[i][a] == 1){
        num_pos++;
        num_neg = 0;
      }else if (m_board[i][a] == -1){
        num_neg++;
        num_pos = 0;  
      }else {
        num_pos = 0;
        num_neg = 0;
      }
    }
  }
  if (num_pos == 5){
    m_gameState = GameState::WinX;
    return;
  }else if (num_neg == 5){
    m_gameState = GameState::WinO;
    return;
  }

  //check vertical
  for(auto k : iter::range(-4, 5)) {
    a = i + k;
    if (a >= 0 && a <= m_N + 1){
      if (num_pos == 5 || num_neg == 5) {break;}
      if(m_board[a][j] == 1){
        num_pos++;
        num_neg = 0;
      }else if (m_board[a][j] == -1){
        num_neg++;
        num_pos = 0;  
      }else {
        num_pos = 0;
        num_neg = 0;
      }
    }
  }
  if (num_pos == 5){
    m_gameState = GameState::WinX;
    return;
  }else if (num_neg == 5){
    m_gameState = GameState::WinO;
    return;
  }

  //check first diagonal
  for(auto k : iter::range(-4, 5)) {
    a = i + k;
    b = j + k;
    if (a >= 0 && a <= m_N + 1){
      if (num_pos == 5 || num_neg == 5) {break;}
      if(m_board[a][b] == 1){
        num_pos++;
        num_neg = 0;
      }else if (m_board[a][b] == -1){
        num_neg++;
        num_pos = 0;  
      }else {
        num_pos = 0;
        num_neg = 0;
      }
    }
  }
  if (num_pos == 5){
    m_gameState = GameState::WinX;
    return;
  }else if (num_neg == 5){
    m_gameState = GameState::WinO;
    return;
  }

  //check second diagonal
  for(auto k : iter::range(-4, 5)) {
    a = i - k;
    b = j + k;
    if (a >= 0 && a <= m_N + 1){
      if (num_pos == 5 || num_neg == 5) {break;}
      if(m_board[a][b] == 1){
        num_pos++;
        num_neg = 0;
      }else if (m_board[a][b] == -1){
        num_neg++;
        num_pos = 0;  
      }else {
        num_pos = 0;
        num_neg = 0;
      }
    }
  }
  if (num_pos == 5){
    m_gameState = GameState::WinX;
    return;
  }else if (num_neg == 5){
    m_gameState = GameState::WinO;
    return;
  }


}
