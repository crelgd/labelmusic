#define GLEW_STATIC
#define SDL_MAIN_HANDLED
#include "GL/glew.h"
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "discord/discord_rpc.h"
#include "game.h"
#include "apiMap.h"
#include "data.h"

#include <stdio.h>
#include <string.h>

#define WW 1280
#define WH 720

void glEnable2D();
void glDisable2D();

char** split(char* text, int* element_count);
void free_split(char** array, int element_count);

DWORD startTimer();
DWORD timerProccess(int startedData);

int main(int argc, char* argv[]) {
    if (argc < 2) return 0;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL INIT\n");
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("LabelMusic", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WW, WH, SDL_WINDOW_OPENGL);
    if (!window) {
        printf("window\n");
        SDL_Quit();
        return 1;
    }

    SDL_GLContext* glcontext = SDL_GL_CreateContext(window);
    if (!glcontext) {
        printf("context\n");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        printf("glew\n");
        return 1;
    }

    Discord_Initialize(DISCORD_APP, NULL, 0, NULL);

    DiscordRichPresence idk = {0};
    idk.state = "Testing";
    idk.largeImageKey = "global_logo";
    idk.largeImageText = "Label Music";

    Discord_UpdatePresence(&idk);

    // text file init
    const char* map_path = argv[1];
    char map_bfr[1024] = {0};

    int map_path_size = strlen(map_path);

    memcpy(map_bfr, map_path, map_path_size * sizeof(char));
    const char* map_bfr_file = "/map.lmm";

    for (int i = 0; i < strlen(map_bfr_file); i++) {
        map_bfr[map_path_size] = map_bfr_file[i];
        map_path_size++;
    }

    //printf("%s\n", map_bfr);

    apiMapModel* api = fapiOpen(map_bfr);
    if (!api) {
        printf("map api\n");
        SDL_GL_DeleteContext(glcontext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int status = fapiCheckMapSign(api);

    printf("status: %d\n", status);

    if (status != 0) {
        printf("map sing\n");
        fapiClose(api);
        SDL_GL_DeleteContext(glcontext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    char* map_song = fapiGetMapSong(api);
    if (!map_song) {
        printf("map song\n");
    }
    char* map_bg = fapiGetMapBGFile(api);
    if (!map_bg) {
        printf("map bg\n");
        fapiClose(api);
        SDL_GL_DeleteContext(glcontext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    char* map_text = fapiGetMapText(api, KEY);
    if (!map_text) {
        printf("map text\n");
        fapiFree(map_bg);
        fapiClose(api);
        SDL_GL_DeleteContext(glcontext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    printf("%s\n", map_text);

    int map_word_count;
    char** map_split_data = split(map_text, &map_word_count);
    if (!map_split_data) {
        printf("map split\n");
        fapiFree(map_text);
        fapiClose(api);
        SDL_GL_DeleteContext(glcontext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    for (int i = 0; i < map_word_count; i++) {
        printf("%s\n", map_split_data[i]);
    }

    fapiFree(map_text);

    if (TTF_Init() != 0) {
        printf("TTF\n");
        return 1;
    }

    game_game_init(map_bg, WW, WH, map_song, strlen(map_song));

    printf("%s\n", map_song);

    fapiFree(map_song);

    fapiFree(map_bg);

    int timing_data_size;
    unsigned char* timing_data_bin = fapiGetMapTimingData(api, &timing_data_size);
    fapiFree(timing_data_bin);

    unsigned int* timing_data = fapiConvertTimingData(timing_data_bin, timing_data_size);

    fapiClose(api);

    int run = 1;

    SDL_Event e;

    int combo = 0;
    int change_combo = 1;

    char buffer[100000];

    DWORD startTimer_data = startTimer();

    DWORD current_time = timerProccess(startTimer_data);
    int time_bfr = 0;

    while (run) {
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                run = 0;
            }
            if (e.type == SDL_KEYDOWN) {
                SDL_Keymod mod = SDL_GetModState();
                SDL_Keycode key = e.key.keysym.sym;

                char current_char = 0x00;

                if (mod & SDLK_LSHIFT) {
                    // Если Shift зажат
                    if (key >= SDLK_a && key <= SDLK_z) {
                        current_char=(char)toupper(key);
                    } else if (key == SDLK_1) {
                        current_char = '!';
                    } else if (key == SDLK_2) {
                        current_char = '@';
                    } else if (key == SDLK_3) {
                        current_char = '#';
                    } else if (key == SDLK_SLASH) {
                        current_char = '?';
                    }
                } else {
                    // Если Shift не зажат
                    if (key >= SDLK_a && key <= SDLK_z) {
                        current_char = (char)key;
                    } 
                    else if (key == SDLK_COMMA) current_char = ',';
                }

                if (key == SDLK_SPACE)current_char = ' ';

                int game = game_game_cliked(current_char, map_split_data);

                if (game == GAME_R) {
                    combo++;
                } else if (game == GAME_L) {
                    combo = 0;
                }
                change_combo = 1;
            }
        }

        current_time = timerProccess(startTimer_data);

        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        game_game_draw_other();

        glEnable2D();
        int res = game_game_draw_text(map_split_data, map_word_count, SDL_itoa(combo, buffer, 10), change_combo, current_time, timing_data, timing_data_size, &time_bfr);
        if (res == GAME_END) {
            run = 0;
        }
        glDisable2D();

        change_combo = 0;

        SDL_GL_SwapWindow(window);
        SDL_Delay(16);
    }

    fapiFree(timing_data);
    game_game_close();

    free_split(map_split_data, map_word_count);
    Discord_ClearPresence();
    Discord_Shutdown();
    TTF_Quit();
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void glEnable2D()
{
	int vPort[4];
  
	glGetIntegerv(GL_VIEWPORT, vPort);
  
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
  
	glOrtho(0, vPort[2], 0, vPort[3], -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void glDisable2D()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();   
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();	
}

char** split(char* text, int* element_count) {
    int text_size = strlen(text);
    int space_count = 0;

    //if (text[0] == ' ') text[0] = 0x00;
    for (int i = 0; i < text_size; i++)
        if (text[i] == ' ') space_count++;

    *element_count = space_count;

    int read_data = 0;
    int word_size = 0;

    int start_pos = 0;
    int start_counter = 0;

    char** output = (char**)malloc(space_count * sizeof(char*));
    if (!output) return NULL;

    for (int i = 0; i < text_size; i++) {
        if (text[i] == ' ' || text[i] == '\n') {
            char* arr = (char*)malloc((word_size+1) * sizeof(char));
            if (!arr) return NULL;

            for (int to_arr = 0; to_arr < word_size; to_arr++) {
                arr[to_arr] = text[start_pos+start_counter];
                start_counter++;
            }
            start_counter = 0;

            arr[word_size] = '\0';

            output[read_data] = arr;

            start_pos = i + 1;   
            word_size = 0;
            read_data++;
            if (read_data >= space_count) 
                break;
        }
        word_size++;
    }

    return output;
}

void free_split(char** array, int element_count) {
    for (int i = 0; i < element_count; i++) {
        free(array[i]);
    }
    free(array);
}

DWORD startTimer() {
    DWORD timestart = GetCurrentTime();
    return timestart;
}

DWORD timerProccess(int startedData) {
    DWORD curTime = GetCurrentTime();
    return curTime - startedData;
}
