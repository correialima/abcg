# Atividade 01: Gomoku

## Rafael Correia de Lima - 21004515

## Descrição

*"O Gomoku, também conhecido como Gobang, é um jogo de tabuleiro estratégico tradicionalmente jogado com peças de Go (peças negras e brancas) em um tabuleiro de Go ligeiramente modificado (15x15 interseções). No entanto, ao contrario deste, uma vez que as peças são colocadas não podem ser movidas ou retiradas do tabuleiro. No inicio do jogo, as negras começam, e os jogadores alternam na colocação de uma pedra da sua cor em uma interseção vazia. O vencedor é o primeiro jogador a obter uma linha ininterrupta de cinco pedras na horizontal, vertical ou diagonal."* [Wikipedia](https://pt.wikipedia.org/wiki/Gomoku)

## Implementação
A implementação do jogo partiu do modelo de jogo da velha disponibilizada nas notas de aula, com algumas modificações para se adequar ao jogo relativamente mais complexo. 

### Lógica do jogo
Diferente do jogo da velha, que es baseava no rótulo dos botões para registrar as alterações de estado, optei por implementar a lógica do jogo separada do comportamento gráfico. O tabuleiro do jogo é representado por uma matriz 15x15 de inteiros. O primeiro jogador é representado como 1, o segundo como -1 e casas vazias como 0. Cada um dos jogadores faz seus movimentos de maneira alternada, podendo colocar uma peça em qualquer uma das casas livres no tabuleiro. A cada jogada realizada é checada a condição de fim de jogo. Para isso é avaliada a submatriz 9x9 centrada na última peça posicionada no tabuleiro, percorrendo a linha horizontal central, vertical central, diagonal principal e secundária, buscando por uma sequência continua de 5 peças do mesmo jogador. Identificado o estado de fim de jogo, não é possível inserir novas peças e é exibida a mensagem com o jogador vencedor. Para se realizar um novo jogo é necessário clicar no botão de reinício de partida.

A estrutura do programa do jogo da velha foi mantida, usando apenas as funções onCreate(), onPaintUI(), checkEndConditions() e restartGame(), com algumas modificações.
* onCreate() não foi modificada;
* restartGame() atribui 0 a todos os campos da matriz que representa o tabuleiro;
* checkEndConditions() verifica o subconjunto da matriz do tabuleiro no qual pode ter sido formada uma sequência continua de 5 inteiros 1 ou -1 ao redor da última peça posicionada no tabuleiro.
* onPaintUI() foi modificada para receber as mudanças visuais feitas em relação ao jogo original e também a mudança na lógica do tabuleiro.

### Aspectos Visuais
Optei por pintar os botões ao invés de aplicar rótulos com X e O nas peças. Cada jogador pode escolher a cor das suas peças a qualquer momento do jogo. Foi mantido o layout do jogo da velha, com a exibilção do turno de cada jogador e da mensagem do jogador ganhador. Foi mantida também o botão de reinício de partida abaixo do tabuleiro.
