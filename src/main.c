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

    if (TTF_Init() != 0) {
        printf("TTF\n");
        return 1;
    }

    int run = 1;

    SDL_Event e;

    int combo = 0;
    int change_combo = 1;

    char buffer[100000];

    while (run) {
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                run = 0;
            }
        }

        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        

        glEnable2D();
        
        glDisable2D();

        change_combo = 0;

        SDL_GL_SwapWindow(window);
        SDL_Delay(16);
    }

    

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
