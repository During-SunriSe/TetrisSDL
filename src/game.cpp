#include "game.h"
#include <iostream>
#include <string>

Game::Game() : window(nullptr), renderer(nullptr), isRunning(true), lastTick(0), tickInterval(500), isGameOver(false), isPaused(false), gameState(MENU) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        isRunning = false;
        return;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        isRunning = false;
        return;
    }

    font = TTF_OpenFont("Sans.ttf", 24);
    if (font == nullptr) {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        isRunning = false;
        return;
    }

    windowWidth = 800;
    windowHeight = 600;
    window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        isRunning = false;
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        SDL_DestroyWindow(window);
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        isRunning = false;
        return;
    }

    restartButtonRect = {0, 0, 200, 50};
    pauseButtonRect = {0, 0, 200, 50};
    playerButtonRect = {(windowWidth - 200) / 2, (windowHeight - 100) / 2 - 30, 200, 50};
    aiButtonRect = {(windowWidth - 200) / 2, (windowHeight - 100) / 2 + 30, 200, 50};
}

Game::~Game() {
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::run() {
    while (isRunning) {
        Uint32 currentTick = SDL_GetTicks();
        if (!isPaused && currentTick - lastTick >= tickInterval && !board.isGameOver()) {
            board.movePieceDown();
            lastTick = currentTick;
        }
        processInput();
        update();
        render();
        SDL_Delay(10); // Small delay to prevent high CPU usage
    }
}

void Game::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        } else if (event.type == SDL_KEYDOWN) {
            if (gameState == PLAYER && !board.isGameOver() && !isPaused) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        board.movePieceLeft();
                        break;
                    case SDLK_RIGHT:
                        board.movePieceRight();
                        break;
                    case SDLK_DOWN:
                        board.movePieceDown();
                        break;
                    case SDLK_UP:
                        board.rotatePiece();
                        break;
                    case SDLK_SPACE:
                        board.dropPiece();
                        break;
                }
            }
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            if (gameState == MENU) {
                handleMenuButtonClick(mouseX, mouseY);
            } else if (board.isGameOver()) {
                handleRestartButtonClick(mouseX, mouseY);
            } else {
                handlePauseButtonClick(mouseX, mouseY);
            }
        }
    }
}

void Game::update() {
    if (gameState == AI && !board.isGameOver() && !isPaused) {
        static int bestX, bestRotation;
        static bool newPiece = true;

        if (newPiece) {
            board.bestMove(bestX, bestRotation);
            newPiece = false;
        }

        Piece currentPiece = board.getCurrentPiece();

        if (currentPiece.position.x < bestX) {
            board.movePieceRight();
        } else if (currentPiece.position.x > bestX) {
            board.movePieceLeft();
        } else {
            for (int r = 0; r < bestRotation; ++r) {
                board.rotatePiece();
            }
            newPiece = true;
        }
    }
    if (board.isGameOver()) {
        isGameOver = true;
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (gameState == MENU) {
        renderMenu();
    } else {
        board.draw(renderer);
        renderScore();

        if (board.isGameOver()) {
            renderGameOver();
            renderRestartButton();
        } else {
            renderPauseButton();
        }
    }

    SDL_RenderPresent(renderer);
}

void Game::renderScore() {
    std::string scoreText = "Score: " + std::to_string(board.getScore());
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font, scoreText.c_str(), white);
    SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect messageRect;
    messageRect.x = windowWidth - surfaceMessage->w - 20; // Adjusted for window width
    messageRect.y = 10;
    messageRect.w = surfaceMessage->w;
    messageRect.h = surfaceMessage->h;
    SDL_RenderCopy(renderer, message, NULL, &messageRect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}

void Game::renderGameOver() {
    std::string gameOverText = "Game Over";
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font, gameOverText.c_str(), white);
    SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect messageRect;
    messageRect.w = surfaceMessage->w;
    messageRect.h = surfaceMessage->h;
    messageRect.x = (windowWidth - messageRect.w) / 64 * 49; // Center horizontally
    messageRect.y = (windowHeight - messageRect.h) / 2 - 30; // Center vertically with offset
    SDL_RenderCopy(renderer, message, NULL, &messageRect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}

void Game::renderRestartButton() {
    std::string restartText = "Restart";
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font, restartText.c_str(), white);
    SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    restartButtonRect.x = (windowWidth - restartButtonRect.w) / 4 * 3; // Center horizontally
    restartButtonRect.y = (windowHeight - restartButtonRect.h) / 2 + 20; // Center vertically with offset

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &restartButtonRect);

    SDL_Rect messageRect;
    messageRect.x = restartButtonRect.x + (restartButtonRect.w - surfaceMessage->w) / 2;
    messageRect.y = restartButtonRect.y + (restartButtonRect.h - surfaceMessage->h) / 2;
    messageRect.w = surfaceMessage->w;
    messageRect.h = surfaceMessage->h;
    SDL_RenderCopy(renderer, message, NULL, &messageRect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}

void Game::renderPauseButton() {
    std::string pauseText = isPaused ? "Resume" : "Pause";
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font, pauseText.c_str(), white);
    SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    pauseButtonRect.x = windowWidth - pauseButtonRect.w - 20; // Adjusted for window width
    pauseButtonRect.y = windowHeight - pauseButtonRect.h - 20; // Adjusted for window height

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(renderer, &pauseButtonRect);

    SDL_Rect messageRect;
    messageRect.x = pauseButtonRect.x + (pauseButtonRect.w - surfaceMessage->w) / 2;
    messageRect.y = pauseButtonRect.y + (pauseButtonRect.h - surfaceMessage->h) / 2;
    messageRect.w = surfaceMessage->w;
    messageRect.h = surfaceMessage->h;
    SDL_RenderCopy(renderer, message, NULL, &messageRect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}

void Game::renderMenu() {
    std::string playerText = "Player";
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font, playerText.c_str(), white);
    SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &playerButtonRect);

    SDL_Rect messageRect;
    messageRect.x = playerButtonRect.x + (playerButtonRect.w - surfaceMessage->w) / 2;
    messageRect.y = playerButtonRect.y + (playerButtonRect.h - surfaceMessage->h) / 2;
    messageRect.w = surfaceMessage->w;
    messageRect.h = surfaceMessage->h;
    SDL_RenderCopy(renderer, message, NULL, &messageRect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);

    std::string aiText = "Algorithm";
    surfaceMessage = TTF_RenderText_Solid(font, aiText.c_str(), white);
    message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(renderer, &aiButtonRect);

    messageRect.x = aiButtonRect.x + (aiButtonRect.w - surfaceMessage->w) / 2;
    messageRect.y = aiButtonRect.y + (aiButtonRect.h - surfaceMessage->h) / 2;
    messageRect.w = surfaceMessage->w;
    messageRect.h = surfaceMessage->h;
    SDL_RenderCopy(renderer, message, NULL, &messageRect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}

void Game::handleRestartButtonClick(int mouseX, int mouseY) {
    if (mouseX >= restartButtonRect.x && mouseY >= restartButtonRect.y &&
        mouseX <= restartButtonRect.x + restartButtonRect.w &&
        mouseY <= restartButtonRect.y + restartButtonRect.h) {
        // Restart the game
        board = Board(); // Reset the board
        isGameOver = false;
        lastTick = SDL_GetTicks(); // Reset the game tick
    }
}

void Game::handlePauseButtonClick(int mouseX, int mouseY) {
    if (mouseX >= pauseButtonRect.x && mouseY >= pauseButtonRect.y &&
        mouseX <= pauseButtonRect.x + pauseButtonRect.w &&
        mouseY <= pauseButtonRect.y + pauseButtonRect.h) {
        // Toggle pause state
        isPaused = !isPaused;
    }
}

void Game::handleMenuButtonClick(int mouseX, int mouseY) {
    if (mouseX >= playerButtonRect.x && mouseY >= playerButtonRect.y &&
        mouseX <= playerButtonRect.x + playerButtonRect.w &&
        mouseY <= playerButtonRect.y + playerButtonRect.h) {
        gameState = PLAYER;
    } else if (mouseX >= aiButtonRect.x && mouseY >= aiButtonRect.y &&
               mouseX <= aiButtonRect.x + aiButtonRect.w &&
               mouseY <= aiButtonRect.y + aiButtonRect.h) {
        gameState = AI;
    }
}
