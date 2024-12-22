#ifndef GAME_MENU_H
#define GAME_MENU_H

// [0x0000, 0x1000]
char** game_menu_get_dir_list(const char* dir, int* dirs_count);

void game_menu_delete_dir_list(char** array, int dirs_count);

void game_menu_init(const char* map_dir);

void game_menu_draw_other();

void game_menu_draw_text();

void game_menu_close();

#endif