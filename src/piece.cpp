#include "piece.h"

Piece::Piece() {
    setType(static_cast<TetrominoType>(rand() % 7));
    setColor(static_cast<TetrominoColor>(rand() % 6));
    position = {0, 0};
}

void Piece::setType(TetrominoType type) {
    this->type = type;
    initializeBlocks();
}

void Piece::setColor(TetrominoColor colorName) {
    switch (colorName) {
        case red:
            color[0] = 255;
            color[1] = 0;
            color[2] = 0;
            break;
        case green:
            color[0] = 0;
            color[1] = 255;
            color[2] = 0;
            break;
        case blue:
            color[0] = 0;
            color[1] = 0;
            color[2] = 255;
            break;
        case yellow:
            color[0] = 255;
            color[1] = 255;
            color[2] = 0;
            break;
        case purple:
            color[0] = 255;
            color[1] = 0;
            color[2] = 255;
            break;
        case aqua:
            color[0] = 0;
            color[1] = 255;
            color[2] = 255;
            break;
    }
}

void Piece::rotate() {
    for (int i = 0; i < 4; ++i) {
        int temp = blocks[i].x;
        blocks[i].x = -blocks[i].y;
        blocks[i].y = temp;
    }
}

void Piece::draw(SDL_Renderer *renderer, int offsetX, int offsetY) {
    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], 255); // Full opacity for current piece
    for (int i = 0; i < 4; ++i) {
        SDL_Rect rect = {
                (blocks[i].x + position.x + offsetX) * BLOCK_SIZE,
                (blocks[i].y + position.y + offsetY) * BLOCK_SIZE,
                BLOCK_SIZE, BLOCK_SIZE
        };
        SDL_RenderFillRect(renderer, &rect);
    }
}

void Piece::initializeBlocks() {
    switch (type) {
        case I:
            blocks[0] = {0, -1}; blocks[1] = {0, 0}; blocks[2] = {0, 1}; blocks[3] = {0, 2};
            break;
        case O:
            blocks[0] = {0, 0}; blocks[1] = {0, 1}; blocks[2] = {1, 0}; blocks[3] = {1, 1};
            break;
        case T:
            blocks[0] = {-1, 0}; blocks[1] = {0, 0}; blocks[2] = {1, 0}; blocks[3] = {0, 1};
            break;
        case S:
            blocks[0] = {-1, 0}; blocks[1] = {0, 0}; blocks[2] = {0, 1}; blocks[3] = {1, 1};
            break;
        case Z:
            blocks[0] = {0, 0}; blocks[1] = {1, 0}; blocks[2] = {-1, 1}; blocks[3] = {0, 1};
            break;
        case J:
            blocks[0] = {-1, 0}; blocks[1] = {0, 0}; blocks[2] = {1, 0}; blocks[3] = {1, 1};
            break;
        case L:
            blocks[0] = {-1, 0}; blocks[1] = {0, 0}; blocks[2] = {1, 0}; blocks[3] = {-1, 1};
            break;
    }
}
