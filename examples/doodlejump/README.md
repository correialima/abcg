# Doodle Jump

Rafael Correia de Lima - 21004515

## Descrição

O jogo desenvolvido é baseado no jogo [Doodle Jump](https://pt.wikipedia.org/wiki/Doodle_Jump).

O objetivo é continuar pulando nas plataformas conforme elas vão descendo. O jogador pode controlar a movimentação horizontal do personagem e este pula automaticamente ao pisar em uma das plataformas. Cada vez que o jogador alcança a parte superior da tela, a velocidade de descida das plataformas e o valor dos pontos obtidos aumentam em 20%. As nuvens tem velocidade de descida constante e servem como obstáculos visuais ao jogador. O jogo acaba quando o jogador cai na parte inferior da tela. Ao final do jogo é exibida uma mensagem de "Game Over!" e o jogo reinicia automaticamente após alguns segundos.


## Implementação

O jogo se baseia em 4 classes que representam a janela do jogo, o jogador, as plataformas e as nuvens. Elas estão definidas abaixo, acompanhadas de breves descrições das funções e atributos relevantes.

* Window
    * checkJump(): verifica se o jogador pulou em uma plataforma, para isso as coordenadas externas de um de seus pés deve estar sob uma das plataformas e sua velocidade vertical deve ser negativa (deve estar caindo). Atualiza-se então o estado do jogador e a contagem dos pontos.
    * checkGameOver(): verifica se o jogador atingiu a parte inferior da tela, caso isso ocorra altera o estado da janela de jogo para GameOver;
    * increaseSpeed(): aumenta a velocidade de descida das plataformas caso o jogador atinja a parte superior da tela.
    * restart(): reinicia o jogo, recriando os elementos necessários
    * score e highScore: guardam o valor do score atual do jogador e o score máximo obtido na execução atual do programa
    * speed: guarda o valor do multiplicador da velocidade e de pontuação
    * m_gameData: estado atual do jogo GameOver ou Playing
    * m_player: objeto representando o jogador
    * m_platforms: objeto contendo as plataformas existentes no jogo
    * m_clouds: objeto contendo as nuvens criadas
    * m_restartWaitTimer: timer de 5 segundos para reinício do jogo após game over
    * speedUpCooldownTimer: timer de 10 segundos, para que o aummento de velocidade só possa ocorrer após esse intervalo
```c++
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

```




* Player
    * paint(): o jogador foi definido como um VBO de 12 vértices combinados através de um EBO
    * jump: atributo usado para identificar o momento em que o jogador colide com uma plataforma
    * m_bottom_left e right: coordenadas externas dos pés do jogador, formam o "hitbox" para identificação de colisões com as plataformas

```c++
class Player {
public:
  void create(GLuint program);
  void paint(GameData const &gameData);
  void destroy();
  void update(GameData const &gameData, float deltaTime);

  // graphic attributes
  glm::vec4 m_color{0.25f,0.0f,0.50f,1.0f};
  float m_scale{0.075f};

  // movement attributes
  std::array<glm::vec2,12> positions;
  glm::vec2 m_translation{};
  glm::vec2 m_velocity{};
  glm::vec2 m_acceleration{};
  bool jump{false};

  // hitbox attributes
  glm::vec2 m_bottom_left{};
  glm::vec2 m_bottom_right{}; 

private:
  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
};
```


* Platforms
    * paint(): as plataformas foram definidas como 4 vérticer formando um retângulo através de GL_TRIANGLE_STRIP
    * m_top_left e right: coordenadas das extremidades da superfície superior das plataformas, usado para itendificar a colisão com os pés do jogador.
    * height: altura relativa das plataformas em relação a plataforma inicial, usada para cálculo da pontuação
    * m_velocity: atributo de velocidade de descida para todas as plataformas
    * makePlatforms: cria uma plataforma, definindo seus parâmetros lógicos e gráficos
    * step_height e horizontal_drift: parâmetros para posicionamento aleatório das plataformas na tela
  
```c++
class Platforms {
public:
  void create(GLuint program,int quantity);
  void paint();
  void destroy();
  void update(const Player &player, float deltaTime);

  // struct for a single platform 
  struct Platform {
    GLuint m_VAO{};
    GLuint m_VBO{};

    // graphics attributes
    glm::vec4 m_color{0.329f,0.208f,0.082f,1.0f};
    float m_scale{0.10f};
    std::array<glm::vec2,4> positions;

    // movement attributes
    glm::vec2 m_translation{};

    float height{}; // used for score calculation

    // hitbox attributes
    glm::vec2 m_top_left{};
    glm::vec2 m_top_right{};
    

  };


  glm::vec2 m_velocity{-0.1f,0.0f}; // movement speed for all platforms
  
  std::list<Platform> m_platforms;

  Platform makePlatform(); 

private:
  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};


  // random attributes for positioning platformsl
  float step_height{-0.7f};
  float horizontal_drift{0.0f};
  std::default_random_engine m_randomEngine;
  std::uniform_real_distribution<float> m_randomDist{-0.8f, 0.8f};

};

```
* Clouds
    * paint(): as nuvens foram definidas como um conjunto de 4 "elipses", consistindo cada uma delas de um GL_TRIANGLE_FAN com 20 triângulos cada, possuem opacidade variada
    * m_velocity: atributo de velocidade de descida para todas as nuvens
    * makePCloud: cria uma nuvem, definindo seus parâmetros lógicos e gráficos
    * step_height e horizontal_drift: parâmetros para posicionamento aleatório das nuvens na tela
  

```c++
class Clouds {
public:
  void create(GLuint program,int quantity);
  void paint();
  void destroy();
  void update(float deltaTime);

  // struct for a single Cloud 
  struct Cloud {
    GLuint m_VAO{};
    GLuint m_VBO{};

    // graphics attributes
    glm::vec4 m_color{1.0f,1.0f,1.0f,0.5f};
    float m_scale{0.35f};

    // movement attributes
    glm::vec2 m_translation{};

  };


  glm::vec2 m_velocity{0.0f,-0.1f}; // movement speed for all clouds
  
  std::list<Cloud> m_clouds;

  Cloud makeCloud(); 

private:
  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};


  // random attributes for positioning platformsl
  float step_height{-0.7f};
  float horizontal_drift{0.0f};
  std::default_random_engine m_randomEngine;
  std::uniform_real_distribution<float> m_randomDist{0.0f, 1.0f};

};
```


