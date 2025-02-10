#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>

#include <raylib.h>

#include "../libs/DynamicString.h"

#define PROGRAM "raypoker"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

typedef enum {
    MAIN_MENU = 0,
    CREATE_DB,
    START_SESSION,
} Game_Screen;

typedef struct {
    DS* items;
    size_t count;
    size_t capacity;
} Entries;

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raypoker");
    SetTargetFPS(60);
    Game_Screen current_screen = MAIN_MENU;
    Font font = LoadFont("../resources/dejavu.fnt");

    const char* home = getenv("HOME");
    if (home == NULL) {
	    fprintf(stderr, "No $HOME\n");
	    return 1;
    }
    DS config_dir = DS_from_cstr("");
    DS_push_back_cstr(&config_dir, home);
    DS_push_back_cstr(&config_dir, "/.config/"PROGRAM);

    DIR* dir = opendir(DS_to_cstr(&config_dir));
    if (dir == NULL) {
        fprintf(stderr, "Couldn't open config_dir %s", DS_to_cstr(&config_dir));
        return 1;
    }

    Entries entries =
    {
        .capacity = 5,
        .count = 0,
    };
    entries.items = malloc(sizeof(entries.items[0]) * entries.capacity);
    for (struct dirent* entry = readdir(dir); entry != NULL; entry = readdir(dir)) {
        DS name = DS_from_cstr(entry->d_name);
        if (DS_eq_cstr(&name, ".") || DS_eq_cstr(&name, "..")) continue;
        if (entries.count >= entries.capacity) {
            entries.capacity *= 2;
            DS* new_entries_items = realloc(entries.items, sizeof(entries.items[0]) * entries.capacity);
            for (size_t i = 0; i < entries.count; i++) {
                DS_free(&entries.items[i]);
            }
            free(entries.items);
            entries.items = new_entries_items;
        }
        entries.items[entries.count++] = DS_from_cstr(entry->d_name);
    }


    const char* btn_create_db_cstr = "Create Database";
    Vector2 btn_create_db_vec2 = MeasureTextEx(font, btn_create_db_cstr, (float) font.baseSize, 1.0);

    Rectangle btn_create_db_rec =
    {
        .width = btn_create_db_vec2.x + 15.0,
        .x = (SCREEN_WIDTH - btn_create_db_vec2.x - 15.0)/2,
        .height = btn_create_db_vec2.y + 15.0,
        .y = (SCREEN_HEIGHT - btn_create_db_vec2.y - 15.0)/2,
    };

    while (!WindowShouldClose()) {
        {
            switch (current_screen) {
            case MAIN_MENU: {
                // Handle button presses
            } break;
            }

        }
	    BeginDrawing();
	    {
	        ClearBackground(WHITE);
            switch (current_screen) {
            case MAIN_MENU: {
                DrawRectangleRec(btn_create_db_rec, RED);
                DrawTextEx(font, btn_create_db_cstr, (Vector2){ .x = (SCREEN_WIDTH - btn_create_db_vec2.x)/2, .y = (SCREEN_HEIGHT - btn_create_db_vec2.y)/2 }, (float) font.baseSize, 1.0, BLACK);
            } break;
            }
	    }
	    EndDrawing();
    }

    UnloadFont(font);
    CloseWindow();
    return 0;
}
