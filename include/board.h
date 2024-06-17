#ifndef BOARD_H
#define BOARD_H

#include "piece.h"

const int BOARD_WIDTH = 10;
const int BOARD_HEIGHT = 20;

struct Cell {
    bool filled;
    Uint8 color[3];
};

class Board {
public:
    Board();
    void draw(SDL_Renderer *renderer);
    void spawnPiece();
    void movePieceLeft();
    void movePieceRight();
    void movePieceDown();
    void dropPiece();
    void rotatePiece();
    bool isPieceFit(const Piece &piece, int x, int y);
    int getScore() const;
    bool isGameOver() const;
    void bestMove(int& bestX, int& bestRotation);  // Updated function signature

    Piece getCurrentPiece() const; // Access method for currentPiece
    int evaluateBoard();  // Add this function to evaluate the board state
    int countHoles();     // Add this function to count the number of holes
    int aggregateHeight(); // Add this function to calculate the aggregate height
    int bumpiness();       // Add this function to calculate the bumpiness

private:
    Cell grid[BOARD_HEIGHT][BOARD_WIDTH];
    Piece currentPiece;
    bool isPieceLocked;
    int score;
    bool gameOver;
    void lockPiece();
    void clearLines();
};

#endif // BOARD_H
