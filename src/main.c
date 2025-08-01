#include "raylib.h"
#include "game.h"

int main(void) {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(800, 450, "Isometric Snake");
    SetTargetFPS(60);

    Texture2D texture = LoadTexture("../res/sprite.png");

    sprite floor = {{32, 128, 32, 32}, {0, 0, TILE_WIDTH, TILE_HEIGHT}};
    apple apple = {(sprite){{32, 192, 32, 32}, {0, 0, TILE_WIDTH, TILE_HEIGHT}}, (Vector2){10, 10}};
    snake snake = {(sprite){{0, 0, 32, 32}, {0, 0, TILE_WIDTH, TILE_HEIGHT}}, (Vector2){0, 0}};

    //offset to center of the screen
    float verticalOffset = (GetScreenHeight() - TILES_Y*TILE_HEIGHT/2)/2;
    float horizontalOffset = GetScreenWidth()/2 - TILE_WIDTH/2;

    float gameSpeed = 0; //used to control speed of the game

    while (!WindowShouldClose()) {
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
        gameSpeed += deltaTime;
        if (gameSpeed >= 0.2) {
            switch (snake.dir)
            {
            case UP:
                snake.pos.y--;
                break;
            case DOWN:
                snake.pos.y++;
                break;
            case RIGHT:
                snake.pos.x++;
                break;
            case LEFT:
                snake.pos.x--;
                break;
            
            default:
                break;
            }
            gameSpeed = 0;
        }
        snake.sprite.src.x = snake.dir*32; //updating snake facing direction 

        BeginDrawing();
        ClearBackground(SKYBLUE);
        for (int y = 0; y < TILES_Y; y++) {
            for (int x = 0; x < TILES_X; x++) {
                floor.dest.x = (MATRIX[0][0] * x) + (MATRIX[0][1] * y) + horizontalOffset;
                floor.dest.y = (MATRIX[1][0] * x) + (MATRIX[1][1] * y) + verticalOffset;

                DrawTexturePro(texture, floor.src, floor.dest, (Vector2){0, 0}, 0, WHITE);
            }
        }

        snake.sprite.dest.x = (MATRIX[0][0] * snake.pos.x) + (MATRIX[0][1] * snake.pos.y) + horizontalOffset;
        snake.sprite.dest.y = (MATRIX[1][0] * snake.pos.x) + (MATRIX[1][1] * snake.pos.y) + verticalOffset;

        apple.sprite.dest.x = (MATRIX[0][0] * apple.pos.x) + (MATRIX[0][1] * apple.pos.y) + horizontalOffset;
        apple.sprite.dest.y = (MATRIX[1][0] * apple.pos.x) + (MATRIX[1][1] * apple.pos.y) + verticalOffset;

        DrawTexturePro(texture, apple.sprite.src, apple.sprite.dest, (Vector2){0, 0}, 0, WHITE);
        DrawTexturePro(texture, snake.sprite.src, snake.sprite.dest, (Vector2){0, 0}, 0, WHITE);

        DrawText(TextFormat("Mouse Position: (%.0f, %.0f)", GetMousePosition().x - horizontalOffset, GetMousePosition().y - verticalOffset), 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Mouse Position trans: (%.0f, %.0f)", INV_MATRIX[0][0]*(GetMousePosition().x-horizontalOffset) + INV_MATRIX[0][1]*(GetMousePosition().y-verticalOffset)-1, INV_MATRIX[1][0]*(GetMousePosition().x-horizontalOffset) + INV_MATRIX[1][1]*(GetMousePosition().y-verticalOffset)), 10, 30, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();

    UnloadTexture(texture);
    return 0;
}
