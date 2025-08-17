#include "raylib.h"
#include "game.h"

int main(void) {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(800, 450, "Isometric Snake");
    SetTargetFPS(60);
    init();

    while (!WindowShouldClose()) {
        switch (currentState) {
        case GAME: updateGame(); break;
        case GAMEOVER: updateGameover(); break;
        default: break;
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);
        switch (currentState) {
        case GAME: drawGame(); break;
        case GAMEOVER: drawGameover(); break;
        default: break;
        }
        EndDrawing();
    }

    CloseWindow();

    UnloadTexture(texture);
    return 0;
}
