// Aqui definiremos as funções membro da classe Window

#include "window.hpp"

void Window::restartGame() { //A função simplesmente preenche o tabuleiro com caracteres nulos e define o estado do jogo como GameState::Play.

  m_board.fill('\0');
  m_gameState = GameState::Play;
}

void Window::onCreate() {
  // Load font with bigger size for the X's and O's
  auto const filename{abcg::Application::getAssetsPath() + // criamos uma string com o caminho completo do arquivo Inconsolata-Medium.ttf. Para isso usamos a função abcg::Application::getAssetsPath() que retorna o caminho atual do subdiretório assets.
                      "Inconsolata-Medium.ttf"};
  m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 72.0f); // carrega a fonte com tamanho 72
  if (m_font == nullptr) {  // se o ponteiro for nulo, cai nesse if (Só será nulo se algum erro ocorrer no processamento anterior)
    throw abcg::RuntimeError{"Cannot load font file"};
  }

  restartGame();
}

void Window::onPaintUI() {
  // Get size of application's window
  auto const appWindowWidth{gsl::narrow<float>(getWindowSettings().width)}; //pega o valor atual da janela da aplicação
  auto const appWindowHeight{gsl::narrow<float>(getWindowSettings().height)};

  // "Tic-Tac-Toe" window
  {
    ImGui::SetNextWindowSize(ImVec2(appWindowWidth, appWindowHeight)); //tamanho da próxima janela é o mesmo tamanho da aplicação
    ImGui::SetNextWindowPos(ImVec2(0, 0)); // a próxima janela será criada na origem

    auto const flags{ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | //propriedades da janela a ser criada
                     ImGuiWindowFlags_NoCollapse}; //não poderá ser colapsada ao clicar na barra de título 
    ImGui::Begin("Tic-Tac-Toe", nullptr, flags);

    // TODO: Add Menu

        // Menu
    {
      bool restartSelected{};   //inicializada com false
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Game")) {
          ImGui::MenuItem("Restart", nullptr, &restartSelected);    // é padrão no ImGui que, ao clicar em um item de menu (como o "Restart"), o estado da variável booleana passada como ponteiro se torna true.
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }
      if (restartSelected) {
        restartGame();
      }
    }
    // TODO: Add static text showing current turn or win/draw messages

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
    // TODO: Add game board

        // Game board
    {
      auto const gridHeight{appWindowHeight - 22 - 60 - (m_N * 10) - 60};   // altura total descontando margens, botões e paddings
      auto const buttonHeight{gridHeight / m_N};    // altura de cada botao

      // Use custom font
      ImGui::PushFont(m_font);  //seta a fonte a ser usada
      if (ImGui::BeginTable("Game board", m_N)) {   // cria uma tabela com m_N colunas
        for (auto i : iter::range(m_N)) {
          ImGui::TableNextRow();
          for (auto j : iter::range(m_N)) {
            ImGui::TableSetColumnIndex(j);
            auto const offset{i * m_N + j};

            // Get current character
            auto ch{m_board.at(offset)};

            // Replace null character with whitespace because the button label
            // cannot be an empty string
            if (ch == 0) {
              ch = ' ';
            }

            // Button text is ch followed by an ID in the format ##ij
            auto buttonText{fmt::format("{}##{}{}", ch, i, j)};
            if (ImGui::Button(buttonText.c_str(), ImVec2(-1, buttonHeight))) {
              if (m_gameState == GameState::Play && ch == ' ') {
                m_board.at(offset) = m_XsTurn ? 'X' : 'O';
                checkEndCondition();
                m_XsTurn = !m_XsTurn;
              }
            }
          }
          ImGui::Spacing();
        }
        ImGui::EndTable();
      }
      ImGui::PopFont(); // reseta a fonte escolhida usada
    }

    ImGui::Spacing();
    // TODO: Add "Restart game" button

        // "Restart game" button
    {
      if (ImGui::Button("Restart game", ImVec2(-1, 50))) {
        restartGame();
      }
    }

    ImGui::End();
  }
}

void Window::checkEndCondition() {
  if (m_gameState != GameState::Play) {
    return;
  }

  // Lambda expression that checks if a string contains only Xs or Os. If so, it
  // changes the game state to WinX or WinO accordingly and returns true.
  // Otherwise, returns false.
  auto allXsOrOs{[&](std::string_view str) {
    if (str == std::string(m_N, 'X')) {
      m_gameState = GameState::WinX;
      return true;
    }
    if (str == std::string(m_N, 'O')) {
      m_gameState = GameState::WinO;
      return true;
    }
    return false;
  }};

  // TODO: Check rows

    // Check rows
  for (auto const i : iter::range(m_N)) {
    std::string concatenation;
    for (auto const j : iter::range(m_N)) {
      concatenation += m_board.at(i * m_N + j);
    }
    if (allXsOrOs(concatenation)) {
      return;
    }
  }
    // Check columns
  for (auto const j : iter::range(m_N)) {
    std::string concatenation;
    for (auto const i : iter::range(m_N)) {
      concatenation += m_board.at(i * m_N + j);
    }
    if (allXsOrOs(concatenation)) {
      return;
    }
  }

  // Check main diagonal
  {
    std::string concatenation;
    for (auto const i : iter::range(m_N)) {
      concatenation += m_board.at(i * m_N + i);
    }
    if (allXsOrOs(concatenation)) {
      return;
    }
  }

  // Check inverse diagonal
  {
    std::string concatenation;
    for (auto const i : iter::range(m_N)) {
      concatenation += m_board.at(i * m_N + (m_N - i - 1));
    }
    if (allXsOrOs(concatenation)) {
      return;
    }
  }

  // Check draw
  if (std::find(m_board.begin(), m_board.end(), '\0') == m_board.end()) {
    m_gameState = GameState::Draw;
  }
}