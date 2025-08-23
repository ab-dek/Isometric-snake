#ifndef GAME_H
#define GAME_H

#include "raylib.h"

//number of tile blocks
#define TILES_X 20
#define TILES_Y 20

//size of a tile
#define TILE_WIDTH 32
#define TILE_HEIGHT 32

#define GAME_SPEED 0.125 // control the game speed, lower is faster

typedef enum { RIGHT, LEFT, DOWN, UP } dir;
typedef enum { GAME, GAMEOVER } gameState;
typedef enum { SNAKE, SEGMENT, APPLE } objType; 

typedef struct { Rectangle src, dest; } Sprite;
typedef struct { Sprite sprite; Vector2 pos; } Apple;
typedef struct Segment Segment;
struct Segment { Vector2 pos; Segment *next, *prev; };
typedef struct { Sprite sprite; Vector2 pos; dir dir; Segment *head, *tail; } Snake;
typedef struct { Vector2* pos; objType type; void* obj; } objWrapper; // a generic wrapper struct used for sorting game objects 

extern gameState currentState;
extern Texture2D texture;

void init();
void updateGame();
void drawGame();
void updateGameover();
void drawGameover();
static Vector2 isoToScreen(Vector2 pos);
static void addSegment();
static void removeSegment();
static void addToList(void* newObj, objType type);
static void removeFromList(void* obj);
static int compare(const void *a, const void *b);
void unloadList();

#endif