#ifndef GAME_H
#define GAME_H

#include "raylib.h"

typedef enum {
    RIGHT = 0, 
    LEFT, 
    DOWN,
    UP 
} dir;

typedef struct { 
    Rectangle src, dest; 
} sprite;

typedef struct { 
    sprite sprite; 
    Vector2 pos; 
    dir dir; 
    float speed;
} snake;

typedef struct { 
    sprite sprite; 
    Vector2 pos; 
} apple;

//number of tile blocks
const int TILES_X = 20;
const int TILES_Y = 20;

//size of a tile
const float TILE_WIDTH = 32;
const float TILE_HEIGHT = 32;

// isometric grid transformation matrix
const float MATRIX[2][2] = {
    {0.5*TILE_WIDTH, -0.5*TILE_WIDTH},
    {0.25*TILE_HEIGHT, 0.25*TILE_HEIGHT}};

// inverse of the matrix(for screen to isometric coordinate transformation)
const float det = (MATRIX[0][0] * MATRIX[1][1]) - (MATRIX[0][1] * MATRIX[1][0]);
const float INV_MATRIX[2][2] = {
    {MATRIX[1][1] / det, -MATRIX[0][1] / det},
    {-MATRIX[1][0] / det, MATRIX[0][0] / det}
};

#endif