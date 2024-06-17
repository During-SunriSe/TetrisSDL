#ifndef PIECE_H
#define PIECE_H

#include <SDL.h>
#include "block.h"


const int BLOCK_SIZE = 30;

enum TetrominoType {
    I, O, T, S, Z, J, L
};

enum TetrominoColor {
    red, green, blue, yellow, purple, aqua
};

class Piece {
public:
    Piece();
    void setType(TetrominoType type);
    void setColor(TetrominoColor color);
    void rotate();
    void draw(SDL_Renderer *renderer, int offsetX, int offsetY);

    TetrominoType type;
    Uint8 color[3]; // RGB color array
    Block blocks[4];
    Block position;

private:
    void initializeBlocks();
};

#endif // PIECE_H
