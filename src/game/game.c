#include "game.h"
#include "GL/glew.h"

#include "SDL/SDL_ttf.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#define MAIN_FONT "data/consola.ttf"

float screenToGLX(int val, int screenWidth) {
    return (val / (float)(screenWidth / 2)) - 1;
}

float screenToGLY(int val, int screenHeight) {
    return 1 - (val / (float)(screenHeight / 2));
}

uapiImage* game_bg;
uapiObject game_bg_smooth;

uapiShader sh;
uapiProgram sh_prog;

GLint shader_color;

uapiText* game_text;

int text_counter = 0;

int text_x;
int text_y;

int new_text = 1;

int tpos = 0;

int gww, gwh;

uapiObject game_text_marker_obj;
uapiText* game_combo_counter;

ma_result result;
ma_engine engine;

char map_song_global[256] = {0};

void game_game_init(char* bg_path, int ww, int wh, char* map_song, int map_song_size_arr) {
    float game_bg_data[] = {
        -1, -1, 0,
        1, -1, 0,
        1, 1, 0,
        -1, 1, 0
    };
    int x, y, cc;
    game_bg = uapiCreateImage(bg_path, &x, &y, &cc, game_bg_data, sizeof(game_bg_data));
    if (!game_bg) {
        printf("image error\n");
        return;
    }

    game_bg_smooth = uapiCreateObject(game_bg_data, sizeof(game_bg_data));
    if (!game_bg_smooth) {
        printf("smooth\n");
        return;
    }
    
    char* shader_fragment_data = uapiLoadFile("shaders/default_color_shader.fsh");
    if (!shader_fragment_data) return;
    sh = uapiCreateShader(API_FRAGMENT_SHADER, shader_fragment_data);
    if (!sh) {
        printf("shader error\n");
        return;
    }

    sh_prog = uapiCreateProgram(NULL, sh);
    if (!sh_prog) {
        printf("link shader error\n");
        return;
    }

    shader_color = glGetUniformLocation(sh_prog, "color");
    if (shader_color == -1) {
        printf("color error\n");
        return;
    }
    uapiFreeElement(shader_fragment_data);
    uapiDeleteShader(sh);

    SDL_Color color = {255, 255, 255, 255};
    game_text = uapiCreateText("idk!", MAIN_FONT, 40, color);

    gww = ww;
    gwh = wh;

    game_combo_counter = uapiCreateText("NULL", MAIN_FONT, 40, color);

    result = ma_engine_init(NULL, &engine);

    for (int i = 0; i < map_song_size_arr; i++) {
        map_song_global[i] = map_song[i];
    }
    printf("%d| %s\n",map_song_size_arr , map_song_global);
    ma_engine_play_sound(&engine, map_song_global, NULL);
}

void game_game_draw_other() {
    uapiDrawImage(game_bg, 4);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    uapiUseShader(sh_prog);
    float bg_smooth_color_data[4] = {0, 0, 0, 0.5};
    glUniform4fv(shader_color, 1, bg_smooth_color_data);

    uapiDrawObject(game_bg_smooth, 4);

    uapiStopUseShader();

    if (game_text_marker_obj != NULL) {
        uapiUseShader(sh_prog);
        float marker_color[4] = {1, 0, 1, 0.7};
        glUniform4fv(shader_color, 1, marker_color);

        uapiDrawObject(game_text_marker_obj, 4);

        uapiStopUseShader();
    }

    glDisable(GL_BLEND);
}

void game_game_text_marker(char** word_array, int pos) {
    if (game_text_marker_obj) {
        uapiDeleteObject(&game_text_marker_obj);
        game_text_marker_obj = NULL;
    }

    char cur_data[2] = {word_array[text_counter][pos], '\0'};

    int w, h;
    TTF_SizeText(game_text->font, cur_data, &w, &h);

    int offset = w * pos;
    int cursor_position = ((gww - game_text->w) / 2) + offset;

    float marker_data[] = {
        screenToGLX(cursor_position, gww), 0.07, 0,
        screenToGLX(cursor_position+23, gww), 0.07, 0,
        screenToGLX(cursor_position+23, gww), -0.07, 0,
        screenToGLX(cursor_position, gww), -0.07, 0
    };

    game_text_marker_obj = uapiCreateObject(marker_data, sizeof(marker_data));
}

int game_game_cliked(char current_symbol, char** word_array) {
    int status = 0xff;

    if (current_symbol != 0x00) {
        status = GAME_L;
        if (current_symbol == word_array[text_counter][tpos]) {
            status = GAME_R;
        }
    }
    int text_size = strlen(word_array[text_counter]);

    if (current_symbol != 0x00) {
        tpos++;
        new_text = 1;
    }
    if (tpos >= text_size) {
        tpos = 0;
        text_counter++;
        status = GAME_NEWTEXT;
    }
    return status;
}

int change_time_data = 1;
int rval1, rval2 = 0;

int game_game_draw_text(char** word_array, int word_array_count, char* combo, int combo_status, DWORD current_time, unsigned int* time_data, int time_data_size, int* time_bfr) {
    if (change_time_data) {
        if (*time_bfr >= time_data_size) {
            return GAME_END;
        } else {
            rval1 = time_data[*time_bfr++];
            rval2 = time_data[*time_bfr++];
        }

        change_time_data = 0; // крч если че пофиксить изменение текста в промежутке
    }

    if (current_time > rval2) {
        new_text = 1;
        tpos = 0;
        text_counter++;
    }

    if (new_text) {
        if (game_text) {
            uapiDeleteText(game_text);
            game_text = NULL;
        }

        if (text_counter < word_array_count) {
            SDL_Color color = {255, 255, 255, 255};
            game_text = uapiCreateText(word_array[text_counter], "data/consola.ttf", 40, color);
            if (!game_text) {
                return;
            }

            text_x = (gww/2) - (game_text->w/2);
            text_y = (gwh/2) - (game_text->h/2);

            game_game_text_marker(word_array, tpos);
        } else {
            printf("END!");
            return GAME_END;
        }

        new_text = 0;
    }

    if (combo_status == 1) {
        if (game_combo_counter) {
            uapiDeleteText(game_combo_counter);
            game_combo_counter = NULL;
        }

        SDL_Color color = {255, 255, 255, 255};
        game_combo_counter = uapiCreateText(combo, MAIN_FONT, 40, color);
    }

    if (game_combo_counter) {
        if (combo != 0)
            uapiDrawText(game_combo_counter, 10, 10);
    }

    if (game_text) {
        uapiDrawText(game_text, text_x, text_y);
    }
}

void game_game_close() {
    ma_engine_uninit(&engine);

    if (game_combo_counter) uapiDeleteText(game_combo_counter);
    if (game_text) uapiDeleteText(game_text);
    uapiDeleteObject(&game_text_marker_obj);
    uapiDeleteProgram(sh_prog);
    uapiDeleteObject(&game_bg_smooth);
    uapiDeleteImage(game_bg);
}
