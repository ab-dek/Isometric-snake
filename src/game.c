#include "game.h"
#include <stdlib.h>
#include <assert.h>

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

//offset values used to center the game screen
float verticalOffset;
float horizontalOffset;

gameState currentState;
Texture2D texture;
Sprite floorSpr;
Snake snake;
Sprite tailSpr;
Apple apple;
float timer; //used to control speed of the game


void init() {
    texture = LoadTexture("../res/sprite.png");

    floorSpr = (Sprite){{32, 128, 32, 32}, {0, 0, TILE_WIDTH, TILE_HEIGHT}};
    tailSpr = (Sprite){{0, 32, 32, 32}, {0, 0, TILE_WIDTH, TILE_HEIGHT}};
    apple = (Apple){(Sprite){{32, 192, 32, 32}, {0, 0, TILE_WIDTH, TILE_HEIGHT}}, (Vector2){10, 10}};
    snake = (Snake){(Sprite){{0, 0, 32, 32}, {0, 0, TILE_WIDTH, TILE_HEIGHT}}, (Vector2){0, 0}};
    
    addTail(); addTail(); addTail(); // setting the initial length to 3

    verticalOffset = (GetScreenHeight() - TILES_Y*TILE_HEIGHT/2)/2;
    horizontalOffset = GetScreenWidth()/2 - TILE_WIDTH/2;

    timer = 0; 

    currentState = GAME;
}

void updateGame() {
    if(IsKeyPressed(KEY_UP) && snake.dir != DOWN) {
        snake.dir = UP;
    }
    if(IsKeyPressed(KEY_DOWN) && snake.dir != UP) {
        snake.dir = DOWN;
    }
    if(IsKeyPressed(KEY_RIGHT) && snake.dir != LEFT) {
        snake.dir = RIGHT;
    }
    if(IsKeyPressed(KEY_LEFT) && snake.dir != RIGHT) {
        snake.dir = LEFT;
    }

    float deltaTime = GetFrameTime();
    timer += deltaTime;
    if (timer >= GAME_SPEED) { 
        addTail();
        removeTail();
        switch (snake.dir) {
        case UP: snake.pos.y--; break;
        case DOWN: snake.pos.y++; break;
        case RIGHT: snake.pos.x++; break; 
        case LEFT: snake.pos.x--; break;
        default: break;
        }
        timer = 0;
    }
    snake.sprite.src.x = snake.dir*32; //updating Snake facing direction 
}

void drawGame() {
    for (int y = 0; y < TILES_Y; y++) {
        for (int x = 0; x < TILES_X; x++) {
            floorSpr.dest.x = (MATRIX[0][0] * x) + (MATRIX[0][1] * y) + horizontalOffset;
            floorSpr.dest.y = (MATRIX[1][0] * x) + (MATRIX[1][1] * y) + verticalOffset;

            DrawTexturePro(texture, floorSpr.src, floorSpr.dest, (Vector2){0, 0}, 0, WHITE);
        }
    }

    snake.sprite.dest.x = (MATRIX[0][0] * snake.pos.x) + (MATRIX[0][1] * snake.pos.y) + horizontalOffset;
    snake.sprite.dest.y = (MATRIX[1][0] * snake.pos.x) + (MATRIX[1][1] * snake.pos.y) + verticalOffset;

    apple.sprite.dest.x = (MATRIX[0][0] * apple.pos.x) + (MATRIX[0][1] * apple.pos.y) + horizontalOffset;
    apple.sprite.dest.y = (MATRIX[1][0] * apple.pos.x) + (MATRIX[1][1] * apple.pos.y) + verticalOffset;

    Segment* current = snake.head;
    while(current != NULL) {
        //TODO: rendering order!
        tailSpr.dest.x = (MATRIX[0][0] * current->pos.x) + (MATRIX[0][1] * current->pos.y) + horizontalOffset;
        tailSpr.dest.y = (MATRIX[1][0] * current->pos.x) + (MATRIX[1][1] * current->pos.y) + verticalOffset;
        DrawTexturePro(texture, tailSpr.src, tailSpr.dest, (Vector2){0, 0}, 0, WHITE);
        current = current->next;
    }

    DrawTexturePro(texture, apple.sprite.src, apple.sprite.dest, (Vector2){0, 0}, 0, WHITE);
    DrawTexturePro(texture, snake.sprite.src, snake.sprite.dest, (Vector2){0, 0}, 0, WHITE);

    DrawText(TextFormat("Mouse Position: (%.0f, %.0f)", GetMousePosition().x - horizontalOffset, GetMousePosition().y - verticalOffset), 10, 10, 20, DARKGRAY);
    DrawText(TextFormat("Mouse Position trans: (%.0f, %.0f)", INV_MATRIX[0][0]*(GetMousePosition().x-horizontalOffset) + INV_MATRIX[0][1]*(GetMousePosition().y-verticalOffset)-1, INV_MATRIX[1][0]*(GetMousePosition().x-horizontalOffset) + INV_MATRIX[1][1]*(GetMousePosition().y-verticalOffset)), 10, 30, 20, DARKGRAY);
}

void updateGameover() {

}

void drawGameover() {

}

static void addTail() {
    Segment* newSeg = (Segment*)malloc(sizeof(Segment));
    assert(newSeg != NULL);
    newSeg->pos.x = snake.pos.x;
    newSeg->pos.y = snake.pos.y;

    if(snake.head == NULL) { // if snake has 0 length
        snake.head = newSeg;
        snake.tail = newSeg;
    } else {
        newSeg->next = snake.head;
        snake.head->prev = newSeg;
        snake.head = newSeg;
    }
}

static void removeTail() {
    if (snake.head == snake.tail) { // Snake has only one segment
        free(snake.head);
        snake.head = NULL;
        snake.tail = NULL;
        return;
    }
    Segment* lastSeg = snake.tail->prev; // second to last segment
    free(snake.tail);
    snake.tail = lastSeg;
    snake.tail->next = NULL;
}