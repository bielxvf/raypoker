#include <stdio.h>
#include <raylib.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// TODO: Use resources/dejavu.fnt

int main(int argc, char** argv) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raypoker");
    SetTargetFPS(144);

    while (!WindowShouldClose()) {
	BeginDrawing();
	{
	    ClearBackground(WHITE);
	    DrawText("Raypoker", SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 20, LIGHTGRAY);
	}
	EndDrawing();
    }

    CloseWindow();
    return 0;
}
