#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onPaintUI() override;

private:
  static int const m_N{3}; // Board size is m_N x m_N

  enum class GameState { Play, Draw, WinX, WinO };
  GameState m_gameState; //estado atual

  bool m_XsTurn{true}; // variável que indica se o turno atual é do jogador de X
  std::array<char, m_N * m_N> m_board{}; // estado do tabuleiro: '\0', 'X' or 'O'    

  ImFont *m_font{};

  void checkEndCondition(); // será usada no final de cada turno para verificar se m_board está em alguma condição de vitória ou empate;
  void restartGame(); //limpar o tabuleiro e iniciar um novo jogo.
};

#endif