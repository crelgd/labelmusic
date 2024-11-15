#include "apiGraphic.h"
#include "GL/glew.h"
#include "stb_image.h"
#include "SDL/SDL_ttf.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

uapiImage* uapiCreateImage(const char* path_to_file, int* x, int* y, int* image_channels, float* triangle_fan_data, int tf_sizeof) {
    uapiImage* image = (uapiImage*)malloc(sizeof(uapiImage));
    if (!image) return NULL;

    image->data_sizeof = tf_sizeof;

    unsigned char* img = stbi_load(path_to_file, x, y, image_channels, 0);
    if (!img) {
        free(image);
        return NULL;
    }

    glGenTextures(1, &image->texture);
    glBindTexture(GL_TEXTURE_2D, image->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLint format;
    if (*image_channels == 4) {
        format = GL_RGBA;
    } else if (*image_channels == 3) {
        format = GL_RGB;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, *x, *y, 0, format, GL_UNSIGNED_BYTE, img);
    stbi_image_free(img);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenBuffers(1, &image->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, image->vbo);
    glBufferData(GL_ARRAY_BUFFER, tf_sizeof, triangle_fan_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return image;
}

float texcoords[] = {
    0, 1, 
    1, 1,
    1, 0, 
    0, 0
};

void uapiDrawImage(uapiImage* image, int vertex_count) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, image->texture);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, image->vbo);
    glVertexPointer(3, GL_FLOAT, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

    glDrawArrays(GL_TRIANGLE_FAN, 0, vertex_count);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

void uapiDeleteImage(uapiImage* image) {
    glDeleteBuffers(1, &image->vbo);
    glDeleteTextures(1, &image->texture);
    free(image);
}

void uapiChangeImageSize(uapiImage* image, float* triangle_fan_data, int tf_sizeof) {
    glBindBuffer(GL_ARRAY_BUFFER, image->vbo);

    if (image->data_sizeof != tf_sizeof) {
        glBufferData(GL_ARRAY_BUFFER, tf_sizeof, triangle_fan_data, GL_STATIC_DRAW);
        image->data_sizeof = tf_sizeof;
    } else glBufferSubData(GL_ARRAY_BUFFER, 0, tf_sizeof, triangle_fan_data);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

uapiObject uapiCreateObject(float* triangle_fan_data, int tf_sizeof) {
    uapiObject obj;

    glGenBuffers(1, &obj);
    glBindBuffer(GL_ARRAY_BUFFER, obj);
    glBufferData(GL_ARRAY_BUFFER, tf_sizeof, triangle_fan_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return obj;
}

void uapiDrawObject(uapiObject obj, int vertex_count) {
    glEnableClientState(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, obj);
    glVertexPointer(3, GL_FLOAT, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_TRIANGLE_FAN, 0, vertex_count);

    glDisableClientState(GL_VERTEX_ARRAY);
}

void uapiDeleteObject(uapiObject* obj) {
    glDeleteBuffers(1, obj);
}

uapiShader uapiCreateShader(int type, char* shader) {
    uapiShader shd = glCreateShader(type);
    glShaderSource(shd, 1, &shader, NULL);
    glCompileShader(shd);

    GLuint ok;
    glGetShaderiv(shd, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        glDeleteShader(shd);
        return NULL;
    }

    return shd;
}

uapiProgram uapiCreateProgram(uapiShader vert_shader, uapiShader frag_shader) {
    GLuint program = glCreateProgram();
    if (vert_shader)
        glAttachShader(program, vert_shader);
    if (frag_shader)
        glAttachShader(program, frag_shader);
    glLinkProgram(program);

    GLuint ok;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        glDeleteProgram(program);
        return NULL;
    }

    return program;
}

void uapiUseShader(uapiProgram program) {
    glUseProgram(program);
}

void uapiStopUseShader() {
    glUseProgram(0);
}

void uapiDeleteShader(uapiShader shader) {
    glDeleteShader(shader);
}

void uapiDeleteProgram(uapiProgram program) {
    glDeleteProgram(program);
}

char* uapiLoadFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) return NULL;

    fseek(file, 0, SEEK_END);
    int filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = (char*)malloc((filesize + 1) * sizeof(char));
    if (!content) {
        fclose(file);
        return NULL;
    }
    content[filesize] = '\0';

    if (fread(content, sizeof(char), filesize, file) != filesize) {
        free(content);
        fclose(file);
        return NULL;
    }

    fclose(file);

    //printf("%s\n", content);

    return content;
}

void uapiFreeElement(void* obj) {
    free(obj);
}

uapiText* uapiCreateText(const char* text, const char* font_path, int text_px, SDL_Color color) {
    uapiText* api = (uapiText*)malloc(sizeof(uapiText));
    if (!api) return NULL;

    api->font = TTF_OpenFont(font_path, text_px);
    if (!api->font) {
        free(api);
        return NULL;
    }

    SDL_Surface* ts = TTF_RenderUTF8_Blended(api->font, text, color);
    if (!ts) {
        TTF_CloseFont(api->font);
        free(api);
        return NULL;
    }

    api->w = ts->w;
    api->h = ts->h;

    glGenTextures(1, &api->texture);
    glBindTexture(GL_TEXTURE_2D, api->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, api->w, api->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, ts->pixels);

    glBindTexture(GL_TEXTURE_2D, 0);

    SDL_FreeSurface(ts);

    return api;
}

void uapiDrawText(uapiText* text, int x, int y) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, text->texture);

    glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); 
			glVertex2f(x, y);
		glTexCoord2f(1.0f, 1.0f); 
			glVertex2f(x + text->w, y);
		glTexCoord2f(1.0f, 0.0f); 
			glVertex2f(x + text->w, y + text->h);
		glTexCoord2f(0.0f, 0.0f); 
			glVertex2f(x, y + text->h);
	glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    glDisable(GL_BLEND);
}

void uapiDeleteText(uapiText* text) {
    TTF_CloseFont(text->font);
    glDeleteTextures(1, &text->texture);
    free(text);
}
