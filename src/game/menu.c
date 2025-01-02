#include <stdlib.h>
#include <Windows.h>
#include "GL/glew.h"

#include "menu.h"

#include "apiGraphic.h"
#include "apiMap.h"

char** game_menu_get_dir_list(const char* dir, int* dir_count) {
    WIN32_FIND_DATAA win_data;
    HANDLE file = FindFirstFileA(dir, &win_data);
    if (file == INVALID_HANDLE_VALUE) return NULL;

    // count files
    int directory_count = 0;

    do {
        if (win_data.cFileName[0] == '.') {
            continue;
        }
        else if (win_data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
            directory_count++;
        }
    } while (FindNextFileA(file, &win_data) != 0);

    *dir_count = directory_count;

    FindClose(file);
    file = FindFirstFileA(dir, &win_data);

    // global_array
    char** global_array = (char**)malloc(directory_count * sizeof(char*));
    if (!global_array) return NULL;

    int glob_array_pos = 0;

    // read to global array
    do {
        if (win_data.cFileName[0] == '.') {
            continue;
        }
        else if (win_data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
            int filename_size = strlen(win_data.cFileName)+1;
            char* filename = (char*)malloc(filename_size * sizeof(char));
            if (!filename) {
                for (int i = 0; i < glob_array_pos; i++) {
                    free(global_array[i]);
                }
                free(global_array);
                return NULL;
            }
            for (int i = 0; i < filename_size; i++) {
                filename[i] = win_data.cFileName[i];
            }
            global_array[glob_array_pos] = filename;

            glob_array_pos++;
        }
    } while (FindNextFileA(file, &win_data) != 0);
    FindClose(file);

    return global_array;
}

void game_menu_delete_dir_list(char** array, int dirs_count) {
    for (int i = 0; i < dirs_count; i++) {
        free(array[i]);
    }
    free(array);
}

int randomNumber(int max_value) {
    return rand() % max_value;
}

char** game_dir = NULL;
int game_dir_count;

uapiImage* menu_bg;
uapiObject menu_bg_op;

uapiObject play_button;

uapiShader color_shader;
uapiProgram color_program;
GLint color;

void game_menu_init(const char* map_dir) {
    game_dir = game_menu_get_dir_list(map_dir, &game_dir_count);
    if (!game_dir) return;

    int randomNum = randomNumber(game_dir_count);
    if (randomNum >= game_dir_count) randomNum-1;

    char dir_bfr[1024] = {0};

    int map_dir_size = strlen(map_dir)-1;
    char* choiced = game_dir[randomNum];
    int choiced_count = strlen(choiced);
    int count = 0;

    int cur_pos;

    for (int i = 0; i < 1024; i++)  {
        if (i < map_dir_size) {
            dir_bfr[i] = map_dir[i];
        } else {
            if (count < choiced_count)
                dir_bfr[i] = choiced[count];
            else {
                cur_pos = i;
                break;
            }
            count++;
        }
    }

    const char* map = "/map.lmm";

    for (int i = 0; i < (strlen(map)+1); i++) {
        if (i > strlen(map)) {
            dir_bfr[cur_pos] = '\0';
        } else {
            dir_bfr[cur_pos] = map[i];
        }
        cur_pos++;
    }

    printf("%s\n", dir_bfr);

    apiMapModel* fmap = fapiOpen(dir_bfr);

    if (fapiCheckMapSign(fmap) != 0) return; 
    
    char* song = fapiGetMapSong(fmap);
    fapiFree(song);
    char* bg = fapiGetMapBGFile(fmap);

    printf("%s\n", bg);

    float game_bg_data[] = {
        -1, -1, 0,
        1, -1, 0,
        1, 1, 0,
        -1, 1, 0
    };

    int x, y, cc;
    menu_bg = uapiCreateImage(bg, &x, &y, &cc, game_bg_data, sizeof(game_bg_data));
    if (!menu_bg) return;
    fapiFree(bg);
    fapiClose(fmap);

    char* color_shader_code = uapiLoadFile("shaders/default_color_shader.fsh");

    color_shader = uapiCreateShader(API_FRAGMENT_SHADER, color_shader_code);
    color_program = uapiCreateProgram(NULL, color_shader);
    uapiDeleteShader(color_shader);

    uapiFreeElement(color_shader_code);

    float button_start_pos[] = {
        -1, -0.1, 0,
        -0.4, -0.1, 0,
        -0.4, 0.1, 0,
        -1, 0.1, 0
    };
    play_button = uapiCreateObject(button_start_pos, sizeof(button_start_pos));

    menu_bg_op = uapiCreateObject(game_bg_data, sizeof(game_bg_data));

    color = glGetUniformLocation(color_program, "color");
    if (!color) return;
}

void game_menu_draw_other() {
    uapiDrawImage(menu_bg, 4);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    uapiUseShader(color_program);
    float bg_op_color[4] = {0, 0, 0, 0.2};
    glUniform4fv(color, 1, bg_op_color);
    uapiDrawObject(menu_bg_op, 4);
    uapiStopUseShader();

    uapiUseShader(color_program);
    float play_color[4] = {1, 1, 1, 0.6};
    glUniform4fv(color, 1, play_color);
    uapiDrawObject(play_button, 4);
    uapiStopUseShader();

    glDisable(GL_BLEND);
}

void game_menu_close() {
    game_menu_delete_dir_list(game_dir, game_dir_count);
    uapiDeleteObject(&menu_bg_op);
    uapiDeleteObject(&play_button);
    uapiDeleteProgram(color_program);
    uapiDeleteImage(menu_bg);
}
