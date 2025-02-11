#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>
#include <stdbool.h>

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

typedef struct {
    const char* text;
    Rectangle rec;
    Color fg_color;
    Color bg_color;
} Button;

Button Button_init(Font font, const char* text, Vector2 pos, Color bg_color, Color fg_color)
{
    Vector2 btn_dims = MeasureTextEx(font, text, (float) font.baseSize, 1.0);
    Button btn =
    {
        .text = text,
        .rec = {
            .width = btn_dims.x + 15.0,
            .x = pos.x - (btn_dims.x + 15.0) / 2.0,
            .height = btn_dims.y + 15.0,
            .y = pos.y,
        },
        .bg_color = bg_color,
        .fg_color = fg_color,
    };

    return btn;
}

void Button_draw(Font font, Button btn)
{
    DrawRectangleRec(btn.rec, btn.bg_color);
    DrawTextEx(font, btn.text, (Vector2){ .x = btn.rec.x + 15.0/2.0, .y = btn.rec.y + 15.0/2.0 }, (float) font.baseSize, 1.0, btn.fg_color);
}

bool is_mouse_on_rec(Rectangle rec)
{
    Vector2 mouse_position = GetMousePosition();
    if (mouse_position.x >= rec.x &&
    mouse_position.x <= rec.x + rec.width &&
    mouse_position.y >= rec.y &&
    mouse_position.y <= rec.y + rec.height) {
        return true;
    }
    return false;
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raypoker");
    SetTargetFPS(60);
    Game_Screen current_screen = MAIN_MENU;
    Font font = LoadFont("../resources/dejavu.fnt");

    const char* home = getenv("HOME");
    if (home == NULL) {
        perror("No $HOME");
	    abort();
    }
    DS config_dir = DS_from_cstr("");
    DS_push_back_cstr(&config_dir, home);
    DS_push_back_cstr(&config_dir, "/.config/"PROGRAM);

    DIR* dir = opendir(DS_to_cstr(&config_dir));
    if (dir == NULL) {
        // @TODO Handle and create config dir
        perror("Couldn't open config dir");
        abort();
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


    Button btn_create_db = Button_init(font, "Create Database", (Vector2){ .x = SCREEN_WIDTH/2.0, .y = SCREEN_HEIGHT/2.0 }, RED, BLACK);
    Button btn_start_session = Button_init(font, "Start Session", (Vector2){ .x = SCREEN_WIDTH/2.0, .y = SCREEN_HEIGHT/2.0 + 15.0 * 3 }, RED, BLACK);
    Button btn_main_menu = Button_init(font, "Main Menu", (Vector2){ .x = SCREEN_WIDTH / 20.0, .y = SCREEN_WIDTH / 40.0 }, RED, BLACK);

    while (!WindowShouldClose()) {
        {
            switch (current_screen) {
            case MAIN_MENU: {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    if (is_mouse_on_rec(btn_create_db.rec)) {
                        current_screen = CREATE_DB;
                    } else if (is_mouse_on_rec(btn_start_session.rec)) {
                        current_screen = START_SESSION;
                    }
                }
            } break;
            case CREATE_DB: {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    if (is_mouse_on_rec(btn_main_menu.rec)) {
                        current_screen = MAIN_MENU;
                    }
                }
            } break;
            case START_SESSION: {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    if (is_mouse_on_rec(btn_main_menu.rec)) {
                        current_screen = MAIN_MENU;
                    }
                }
            } break;
            }
        }
	    BeginDrawing();
	    {
	        ClearBackground(WHITE);
            switch (current_screen) {
            case MAIN_MENU: {
                Button_draw(font, btn_create_db);
                Button_draw(font, btn_start_session);
            } break;
            case CREATE_DB: {
                Button_draw(font, btn_main_menu);
            } break;
            case START_SESSION: {
                Button_draw(font, btn_main_menu);
            } break;
            }
	    }
	    EndDrawing();
    }

    UnloadFont(font);
    CloseWindow();
    return 0;
}
