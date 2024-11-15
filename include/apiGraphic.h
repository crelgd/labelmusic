#ifndef API_GRAPHIC_H
#define API_GRAPHIC_H

#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

typedef struct uIMAGE {
    unsigned int texture;
    unsigned int vbo;
    int data_sizeof;
} uapiImage;

typedef struct uTEXT {
    unsigned int texture;
    int w;
    int h;
    TTF_Font* font;
} uapiText;

typedef unsigned int uapiObject;
typedef unsigned int uapiShader;
typedef unsigned int uapiProgram;

uapiImage* uapiCreateImage(const char* path_to_file, int* x, int* y, int* image_channels, 
    float* triangle_fan_data, int tf_sizeof);
void uapiDrawImage(uapiImage* image, int vertex_count);
void uapiDeleteImage(uapiImage* image);
void uapiChangeImageSize(uapiImage* image, float* triangle_fan_data, int tf_sizeof);

uapiObject uapiCreateObject(float* triangle_fan_data, int tf_sizeof);
void uapiDrawObject(uapiObject obj, int vertex_count);
void uapiDeleteObject(uapiObject* obj);

#define API_FRAGMENT_SHADER 0x8B30
#define API_VERTEX_SHADER   0x8B31

uapiShader uapiCreateShader(int type, char* shader);
uapiProgram uapiCreateProgram(uapiShader vert_shader, uapiShader frag_shader);
void uapiUseShader(uapiProgram program);
void uapiStopUseShader();
void uapiDeleteShader(uapiShader shader);
void uapiDeleteProgram(uapiProgram program);

char* uapiLoadFile(const char* path);

void uapiFreeElement(void* obj);

uapiText* uapiCreateText(const char* text, const char* font_path, int text_px, SDL_Color color);
void uapiDrawText(uapiText* text, int x, int y);
void uapiDeleteText(uapiText* text);

#endif
