#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "board.h"

enum GameState {
    MENU,
    PLAYER,
    AI
};

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    void processInput();
    void update();
    void render();
    void renderScore();
    void renderGameOver();
    void renderRestartButton();
    void renderPauseButton();
    void renderMenu();
    void handleRestartButtonClick(int mouseX, int mouseY);
    void handlePauseButtonClick(int mouseX, int mouseY);
    void handleMenuButtonClick(int mouseX, int mouseY);

    SDL_Window *window;
    SDL_Renderer *renderer;
    bool isRunning;
    Uint32 lastTick;
    const Uint32 tickInterval;

    Board board;
    TTF_Font *font;

    SDL_Rect restartButtonRect;
    SDL_Rect pauseButtonRect;
    SDL_Rect playerButtonRect;
    SDL_Rect aiButtonRect;
    bool isGameOver;
    bool isPaused;
    GameState gameState;
    int windowWidth;
    int windowHeight;
};

#endif // GAME_H
