#include "raylib.h"

typedef struct { Rectangle src, dest; } sprite;
typedef struct { sprite sprite; Vector2 pos; } snake;

int main(void) {
    //number of tile blocks
    int tilesX = 20;
    int tilesY = 20;

    //size of a tile
    float tileWidth = 32;
    float tileHeight = 32;
    
    // isometric grid transformation matrix
    float matrix[2][2] = {
        {0.5*tileWidth, -0.5*tileWidth},
        {0.25*tileHeight, 0.25*tileHeight}};
    
    // computing the inverse of the matrix
    float invMatrix[2][2];
    float det = (matrix[0][0] * matrix[1][1]) - (matrix[0][1] * matrix[1][0]);
    invMatrix[0][0] = matrix[1][1] / det;
    invMatrix[0][1] = -matrix[0][1] / det;
    invMatrix[1][0] = -matrix[1][0] / det;
    invMatrix[1][1] = matrix[0][0] / det;

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(800, 450, "Isometric Snake");
    SetTargetFPS(60);

    Texture2D texture = LoadTexture("res/sprite.png");

    sprite floor = {{32, 128, 32, 32}, {0, 0, tileWidth, tileHeight}};
    sprite apple = {{32, 192, 32, 32}, {0, 0, tileWidth, tileHeight}};
    snake snake = {(sprite){{32, 0, 32, 32}, {0, 0, tileWidth, tileHeight}}, (Vector2){0, 0}};

    //offset to center of the screen
    float verticalOffset = (GetScreenHeight() - tilesY*tileHeight/2)/2;
    float horizontalOffset = GetScreenWidth()/2 - tileWidth/2;

    while (!WindowShouldClose()) {
        if(IsKeyPressed(KEY_UP)) {
            snake.pos.y--;
        }
        if(IsKeyPressed(KEY_DOWN)) {
            snake.pos.y++;
        }
        if(IsKeyPressed(KEY_RIGHT)) {
            snake.pos.x++;
        }
        if(IsKeyPressed(KEY_LEFT)) {
            snake.pos.x--;
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);
        for (int y = 0; y < tilesY; y++) {
            for (int x = 0; x < tilesX; x++) {
                floor.dest.x = (matrix[0][0] * x) + (matrix[0][1] * y) + horizontalOffset;
                floor.dest.y = (matrix[1][0] * x) + (matrix[1][1] * y) + verticalOffset;

                DrawTexturePro(texture, floor.src, floor.dest, (Vector2){0, 0}, 0, WHITE);
            }
        }

        snake.sprite.dest.x = (matrix[0][0] * snake.pos.x) + (matrix[0][1] * snake.pos.y) + horizontalOffset;
        snake.sprite.dest.y = (matrix[1][0] * snake.pos.x) + (matrix[1][1] * snake.pos.y) + verticalOffset;

        DrawTexturePro(texture, snake.sprite.src, snake.sprite.dest, (Vector2){0, 0}, 0, WHITE);

        DrawText(TextFormat("Mouse Position: (%.0f, %.0f)", GetMousePosition().x - horizontalOffset, GetMousePosition().y - verticalOffset), 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Mouse Position trans: (%.0f, %.0f)", invMatrix[0][0]*(GetMousePosition().x-horizontalOffset) + invMatrix[0][1]*(GetMousePosition().y-verticalOffset)-1, invMatrix[1][0]*(GetMousePosition().x-horizontalOffset) + invMatrix[1][1]*(GetMousePosition().y-verticalOffset)), 10, 30, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();

    UnloadTexture(texture);
    return 0;
}
