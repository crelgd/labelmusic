#ifndef LABELMUSIC_GAME_H
#define LABELMUSIC_GAME_H

#include "apiGraphic.h"

typedef unsigned char* byte_api;

float screenToGLX(int screenX, int screenWidth);
float screenToGLY(int screenY, int screenHeight);

void game_game_init(char* bg_path, int ww, int wh, char* map_song, int map_song_size_arr);

void game_game_draw_other();

#define GAME_END 5

int game_game_draw_text(char** word_array, int word_array_count, char* combo, int combo_status);

void game_game_close();

#define GAME_R       0
#define GAME_L       1
#define GAME_NEWTEXT 2

int game_game_cliked(char current_symbol, char** word_array);

#endif