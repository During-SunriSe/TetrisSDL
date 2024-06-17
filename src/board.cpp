#include "board.h"
#include <cstring>
#include <algorithm>
#include <climits>

Board::Board() : isPieceLocked(false), score(0), gameOver(false) {
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            grid[y][x].filled = false;
            grid[y][x].color[0] = 0;
            grid[y][x].color[1] = 0;
            grid[y][x].color[2] = 0;
        }
    }
    spawnPiece();
}

void Board::draw(SDL_Renderer *renderer) {
    // Draw the game field
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            if (grid[y][x].filled) {
                SDL_SetRenderDrawColor(renderer, grid[y][x].color[0], grid[y][x].color[1], grid[y][x].color[2], 128); // Half opacity
                SDL_Rect rect = {x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    // Draw the current piece
    currentPiece.draw(renderer, 0, 0);

    // Draw the top line and right boundary
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color for the top line
    SDL_RenderDrawLine(renderer, 0, 0, BOARD_WIDTH * BLOCK_SIZE, 0);
    SDL_RenderDrawLine(renderer, BOARD_WIDTH * BLOCK_SIZE, 0, BOARD_WIDTH * BLOCK_SIZE, BOARD_HEIGHT * BLOCK_SIZE);
}


void Board::spawnPiece() {
    currentPiece.setType(static_cast<TetrominoType>(rand() % 7));
    currentPiece.setColor(static_cast<TetrominoColor>(rand() % 6));
    currentPiece.position = {BOARD_WIDTH / 2, -2}; // Initially place the piece higher

    // Check for game over condition
    if (!isPieceFit(currentPiece, currentPiece.position.x, currentPiece.position.y + 2)) {
        gameOver = true;
    } else {
        currentPiece.position.y = 0; // Set the position to the top row
    }

    isPieceLocked = false;
}

void Board::movePieceLeft() {
    if (gameOver) return;
    currentPiece.position.x--;
    if (!isPieceFit(currentPiece, currentPiece.position.x, currentPiece.position.y)) {
        currentPiece.position.x++;
    }
}

void Board::movePieceRight() {
    if (gameOver) return;
    currentPiece.position.x++;
    if (!isPieceFit(currentPiece, currentPiece.position.x, currentPiece.position.y)) {
        currentPiece.position.x--;
    }
}

void Board::movePieceDown() {
    if (gameOver) return;
    currentPiece.position.y++;
    if (!isPieceFit(currentPiece, currentPiece.position.x, currentPiece.position.y)) {
        currentPiece.position.y--;
        lockPiece();
    }
}

void Board::dropPiece() {
    if (gameOver) return;
    while (isPieceFit(currentPiece, currentPiece.position.x, currentPiece.position.y + 1)) {
        currentPiece.position.y++;
    }
    lockPiece();
}

void Board::rotatePiece() {
    if (gameOver) return;
    currentPiece.rotate();
    if (!isPieceFit(currentPiece, currentPiece.position.x, currentPiece.position.y)) {
        currentPiece.rotate();
        currentPiece.rotate();
        currentPiece.rotate(); // Rotate back
    }
}

bool Board::isPieceFit(const Piece &piece, int x, int y) {
    for (int i = 0; i < 4; ++i) {
        int newX = piece.blocks[i].x + x;
        int newY = piece.blocks[i].y + y;
        if (newX < 0 || newX >= BOARD_WIDTH || newY >= BOARD_HEIGHT) {
            return false;
        }
        if (newY >= 0 && grid[newY][newX].filled) { // Check only if newY is non-negative
            return false;
        }
    }
    return true;
}

void Board::lockPiece() {
    for (int i = 0; i < 4; ++i) {
        int x = currentPiece.blocks[i].x + currentPiece.position.x;
        int y = currentPiece.blocks[i].y + currentPiece.position.y;
        if (y >= 0) { // Ensure we do not access negative indices
            grid[y][x].filled = true;
            grid[y][x].color[0] = currentPiece.color[0];
            grid[y][x].color[1] = currentPiece.color[1];
            grid[y][x].color[2] = currentPiece.color[2];
        }
    }
    clearLines();
    spawnPiece();
}

void Board::clearLines() {
    int linesCleared = 0;
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        bool isLineFull = true;
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            if (!grid[y][x].filled) {
                isLineFull = false;
                break;
            }
        }
        if (isLineFull) {
            linesCleared++;
            for (int j = y; j > 0; --j) {
                for (int x = 0; x < BOARD_WIDTH; ++x) {
                    grid[j][x] = grid[j - 1][x];
                }
            }
            for (int x = 0; x < BOARD_WIDTH; ++x) {
                grid[0][x].filled = false;
                grid[0][x].color[0] = 0;
                grid[0][x].color[1] = 0;
                grid[0][x].color[2] = 0;
            }
        }
    }

    // Update score based on lines cleared
    switch (linesCleared) {
        case 1:
            score += 100;
            break;
        case 2:
            score += 300;
            break;
        case 3:
            score += 500;
            break;
        case 4:
            score += 800;
            break;
        default:
            break;
    }
}

int Board::getScore() const {
    return score;
}

bool Board::isGameOver() const {
    return gameOver;
}

int Board::evaluateBoard() {
    int numHoles = countHoles();
    int totalHeight = aggregateHeight();
    int totalBumpiness = bumpiness();
    int clearedLines = getScore();

    return -0.5 * totalHeight + 0.76 * clearedLines - 0.35 * numHoles - 0.18 * totalBumpiness;
}

int Board::countHoles() {
    int holes = 0;
    for (int x = 0; x < BOARD_WIDTH; ++x) {
        bool blockFound = false;
        for (int y = 0; y < BOARD_HEIGHT; ++y) {
            if (grid[y][x].filled) {
                blockFound = true;
            } else if (blockFound && !grid[y][x].filled) {
                holes++;
            }
        }
    }
    return holes;
}

int Board::aggregateHeight() {
    int height = 0;
    for (int x = 0; x < BOARD_WIDTH; ++x) {
        for (int y = 0; y < BOARD_HEIGHT; ++y) {
            if (grid[y][x].filled) {
                height += BOARD_HEIGHT - y;
                break;
            }
        }
    }
    return height;
}

int Board::bumpiness() {
    int bumpiness = 0;
    int previousHeight = 0;

    for (int x = 0; x < BOARD_WIDTH; ++x) {
        int currentHeight = 0;
        for (int y = 0; y < BOARD_HEIGHT; ++y) {
            if (grid[y][x].filled) {
                currentHeight = BOARD_HEIGHT - y;
                break;
            }
        }
        if (x > 0) {
            bumpiness += abs(currentHeight - previousHeight);
        }
        previousHeight = currentHeight;
    }

    return bumpiness;
}

void Board::bestMove(int& bestX, int& bestRotation) {
    int bestScore = INT_MIN;

    for (int rotation = 0; rotation < 4; ++rotation) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            Piece testPiece = currentPiece;
            testPiece.position.x = x;

            for (int r = 0; r < rotation; ++r) {
                testPiece.rotate();
            }

            if (isPieceFit(testPiece, x, 0)) {
                int y = 0;
                while (isPieceFit(testPiece, x, y + 1)) {
                    y++;
                }
                testPiece.position.y = y;

                // Simulate placing the piece
                for (int i = 0; i < 4; ++i) {
                    int px = testPiece.blocks[i].x + testPiece.position.x;
                    int py = testPiece.blocks[i].y + testPiece.position.y;
                    if (py >= 0) {
                        grid[py][px].filled = true;
                    }
                }

                int score = evaluateBoard();

                // Undo the placing of the piece
                for (int i = 0; i < 4; ++i) {
                    int px = testPiece.blocks[i].x + testPiece.position.x;
                    int py = testPiece.blocks[i].y + testPiece.position.y;
                    if (py >= 0) {
                        grid[py][px].filled = false;
                    }
                }

                if (score > bestScore) {
                    bestScore = score;
                    bestX = x;
                    bestRotation = rotation;
                }
            }
        }
    }
}

Piece Board::getCurrentPiece() const {
    return currentPiece;
}
