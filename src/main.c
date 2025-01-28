#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define PROGRAM "raypoker"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define DEFAULT_SPACING 1.0
#define INPUT_BUFFER_SIZE 1024
#define CONFIG_DIR_SIZE 1024
#define FULL_PATH_SIZE 1042

#define UNUSED(x) do { (void) x; } while(false);


void DrawTextCenteredEx(Font font, char* text, float y, float font_size, float spacing, Color color);
bool GuiButtonCentered(float y, char* text, float font_size);


void DrawTextCenteredEx(Font font, char* text, float y, float font_size, float spacing, Color color)
{
    Vector2 text_size_vec = MeasureTextEx(font, text, font_size, spacing);
    Vector2 pos = {.x = (SCREEN_WIDTH - text_size_vec.x)/2, .y = y - (text_size_vec.y)/2};
    DrawTextEx(font, text, pos, font_size, spacing, color);
}

bool GuiButtonCentered(float y, char* text, float font_size)
{
    Font font = GuiGetFont();
    Vector2 text_size_vec = MeasureTextEx(font, text, font_size, DEFAULT_SPACING);
    Rectangle rec_centered = {.x = (SCREEN_WIDTH - text_size_vec.x)/2, .y = y - (text_size_vec.y)/2, .width = text_size_vec.x, .height = text_size_vec.y};
    return GuiButton(rec_centered, text);
}

int main(int argc, char** argv) {
    UNUSED(argc);
    UNUSED(argv);

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raypoker");
    SetTargetFPS(144);

    // TODO: Somehow make this universal
    // TODO: Font scaling (use ttf?)
    Font font_default = LoadFont("../resources/dejavu.fnt");
    GuiSetFont(font_default);

    bool show_mainmenu = true;
    bool show_newdatabase = false;
    bool show_selectdatabase = false;
    bool failed_to_create_file = false;

    char input_buffer[INPUT_BUFFER_SIZE] = {0};

    // TODO: Check for ~/.config/raypoker and
    //       read the entries in a buffer so we
    //       can display them in the main menu

    const char* home = getenv("HOME");
    if (home == NULL) {
	fprintf(stderr, "oooo husbant you gamble all our savings now we homeress\n");
	return 1;
    }
    char config_dir[CONFIG_DIR_SIZE];
    snprintf(config_dir, CONFIG_DIR_SIZE, "%s/.config/"PROGRAM, home);

    DIR* dir = opendir(config_dir);
    if (dir == NULL) {
	fprintf(stderr, "Unable to open config directory \"%s\"\n", config_dir);
	return 1;
    }

    size_t entries_size = 10;
    char** entries = malloc(entries_size * sizeof(char*));
    if (entries == NULL) {
	fprintf(stderr, "out of memory: -9999999 aura\n");
	closedir(dir);
	return 1;
    }

    struct dirent* entry;
    size_t entry_count = 0;

    entry = readdir(dir);
    while ((entry = readdir(dir)) != NULL) {
	if (entry_count >= entries_size) {
	    entries_size *= 1.5;
	    char** new_entries = realloc(entries, entries_size * sizeof(char*));
	    if (new_entries == NULL) {
		fprintf(stderr, "out of memory: -9999999 aura\n");
		for (size_t i = 0; i < entry_count; i++) {
		    free(entries[i]);
		}
		free(entries);
		closedir(dir);
		return 1;
	    }
	    entries = new_entries;
	}

	entries[entry_count] = strdup(entry->d_name);
	if (entries[entry_count] == NULL) {
	    fprintf(stderr, "Memory allocation for entry name failed\n");
	    closedir(dir);
	    return 1;
	}

	entry_count++;
    }

    while (!WindowShouldClose()) {
	BeginDrawing();
	{
	    ClearBackground(WHITE);

	    if (show_mainmenu) {
		DrawTextCenteredEx(font_default, "Main menu", 32, (float) font_default.baseSize, DEFAULT_SPACING, DARKGRAY);

		if (GuiButtonCentered(70.0, "New database", (float) font_default.baseSize)) {
		    show_mainmenu = false;
		    show_newdatabase = true;
		    continue;
		}

		// TODO: Show all .json databases in config_dir
	    } else if (show_newdatabase) {
		DrawTextCenteredEx(font_default, "Create new database", 32, (float) font_default.baseSize, DEFAULT_SPACING, DARKGRAY);

		GuiTextBox((Rectangle){.x = (SCREEN_WIDTH - 350.0)/2, .y = 70.0, .width = 350.0, .height = 45.0}, input_buffer, INPUT_BUFFER_SIZE, true);
		if (GuiButtonCentered(125.0, "Create database", (float) font_default.baseSize)) {
		    char full_path[FULL_PATH_SIZE];
		    snprintf(full_path, FULL_PATH_SIZE, "%s/%s.json", config_dir, input_buffer);
		    FILE* file = fopen(full_path, "wx");
		    if (file == NULL) {
			failed_to_create_file = true;
		    } else {
			failed_to_create_file = false;
			memset(input_buffer, 0, INPUT_BUFFER_SIZE);
			// TODO: Update db entries
			show_newdatabase = false;
			show_mainmenu = true;
			fclose(file);
			continue;
		    }
		}

		if (failed_to_create_file) {
		    DrawTextCenteredEx(font_default, "Failed to create file!", SCREEN_HEIGHT/2, (float) font_default.baseSize, DEFAULT_SPACING, RED);
		}

		if (GuiButtonCentered(SCREEN_HEIGHT - 32, "Back to main menu", (float) font_default.baseSize)) {
		    memset(input_buffer, 0, INPUT_BUFFER_SIZE);
		    failed_to_create_file = false;
		    show_newdatabase = false;
		    show_mainmenu = true;
		    continue;
		}
	    }
	}
	EndDrawing();
    }

    for (size_t i = 0; i < entries_size; i++) {
	free(entries[i]);
    }
    UnloadFont(font_default);
    CloseWindow();
    return 0;
}
