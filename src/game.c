#include "game.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

objWrapper* objects; // list of game objects to be sorted and rendered
size_t count = 0; //number of items in objects
size_t capacity = 4; //memory allocated

void init() {
    objects = (objWrapper*)malloc(capacity*sizeof(objWrapper));
    if (objects == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    texture = LoadTexture("../res/sprite.png");

    floorSpr = (Sprite){{32, 128, 32, 32}, {0, 0, TILE_WIDTH, TILE_HEIGHT}};
    tailSpr = (Sprite){{0, 32, 32, 32}, {0, 0, TILE_WIDTH, TILE_HEIGHT}};
    apple = (Apple){(Sprite){{32, 192, 32, 32}, {0, 0, TILE_WIDTH, TILE_HEIGHT}}, (Vector2){10, 10}};
    snake = (Snake){(Sprite){{0, 0, 32, 32}, {0, 0, TILE_WIDTH, TILE_HEIGHT}}, (Vector2){0, 0}};
    
    addToList(&snake, SNAKE);
    addToList(&apple, APPLE);
    addSegment(); addSegment(); addSegment(); // setting the initial length to 3

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
        addSegment();
        removeSegment();
        switch (snake.dir) {
        case UP: snake.pos.y--; break;
        case DOWN: snake.pos.y++; break;
        case RIGHT: snake.pos.x++; break; 
        case LEFT: snake.pos.x--; break;
        default: break;
        }
        timer = 0;
        if(count > 1){
            qsort(objects, count, sizeof(objWrapper), compare);
        }
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

    for (int i = 0; i < count; i++) {
        objWrapper obj = objects[i];
        switch (obj.type) {
            case SNAKE: {
                Snake* snake = (Snake*)obj.obj;
                snake->sprite.dest.x = isoToScreen(snake->pos).x;
                snake->sprite.dest.y = isoToScreen(snake->pos).y;
                DrawTexturePro(texture, snake->sprite.src, snake->sprite.dest, (Vector2){0, 0}, 0, WHITE);
                break;
            }
            case SEGMENT: {
                Segment* seg = (Segment*)obj.obj;
                tailSpr.dest.x = isoToScreen(seg->pos).x;
                tailSpr.dest.y = isoToScreen(seg->pos).y;
                DrawTexturePro(texture, tailSpr.src, tailSpr.dest, (Vector2){0, 0}, 0, WHITE);
                break;
            }
            case APPLE: {
                Apple* apple = (Apple*)obj.obj;
                apple->sprite.dest.x = isoToScreen(apple->pos).x;
                apple->sprite.dest.y = isoToScreen(apple->pos).y;
                DrawTexturePro(texture, apple->sprite.src, apple->sprite.dest, (Vector2){0, 0}, 0, WHITE);
                break;
            }
        }
    }

    DrawText(TextFormat("Mouse Position: (%.0f, %.0f)", GetMousePosition().x - horizontalOffset, GetMousePosition().y - verticalOffset), 10, 10, 20, DARKGRAY);
    DrawText(TextFormat("Mouse Position trans: (%.0f, %.0f)", INV_MATRIX[0][0]*(GetMousePosition().x-horizontalOffset) + INV_MATRIX[0][1]*(GetMousePosition().y-verticalOffset)-1, INV_MATRIX[1][0]*(GetMousePosition().x-horizontalOffset) + INV_MATRIX[1][1]*(GetMousePosition().y-verticalOffset)), 10, 30, 20, DARKGRAY);
}

void updateGameover() {

}

void drawGameover() {

}

static Vector2 isoToScreen(Vector2 isoPos) {
    int x = (MATRIX[0][0] * isoPos.x) + (MATRIX[0][1] * isoPos.y) + horizontalOffset;
    int y = (MATRIX[1][0] * isoPos.x) + (MATRIX[1][1] * isoPos.y) + verticalOffset;
    return (Vector2){x, y};
}

static void addSegment() { // adds a new segment in front
    Segment* newSeg = (Segment*)malloc(sizeof(Segment));
    if (newSeg == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
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

    addToList(newSeg, SEGMENT);
}

static void removeSegment() { // removes the last segment
    if (snake.head == snake.tail) { // Snake has only one segment
        free(snake.head);
        snake.head = NULL;
        snake.tail = NULL;
        return;
    }
    Segment* lastSeg = snake.tail->prev; // second to last segment
    free(snake.tail);
    removeFromList(snake.tail);
    snake.tail = lastSeg;
    snake.tail->next = NULL;
}

static void addToList(void* newObj, objType type) { // add an object to the objects array
    if(count == capacity) {
        capacity = capacity*2;
        objWrapper* temp = realloc(objects, capacity*sizeof(objWrapper));
        if (temp == NULL) {
            perror("Failed to reallocate memory");
            exit(EXIT_FAILURE);
        }
        objects = temp;
    }

    objWrapper obj;
    obj.type = type;
    obj.obj = newObj;
    switch(type) {
        case SNAKE: obj.pos = &(((Snake*)newObj)->pos); break;
        case SEGMENT: obj.pos = &(((Segment*)newObj)->pos); break;
        case APPLE: obj.pos = &(((Apple*)newObj)->pos); break;
    }
    objects[count] = obj;
    count++;
}

static void removeFromList(void* obj) {
    int i, isFound = -1;

    for (i = 0; i < count; i++) {
        if (objects[i].obj == obj) {
            isFound = i;
            break;
        }
    }

    if(isFound != -1) {
        memmove(&objects[i], &objects[i+1], (count-1-i) * sizeof(objWrapper));
        count--;
    }
}

void unloadList() {
    free(objects);
    objects = NULL;
    count = 0;
    capacity = 4;
}

static int compare(const void *a, const void *b) {
    objWrapper* itemA = (objWrapper*)a;
    objWrapper* itemB = (objWrapper*)b;

    float sumA = itemA->pos->x + itemA->pos->y;
    float sumB = itemB->pos->x + itemB->pos->y;

    if (sumA < sumB) return -1;
    if (sumA > sumB) return 1;
    return 0;
}