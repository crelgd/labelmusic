#ifndef API_MAP_H
#define API_MAP_H

#include <stdio.h>

typedef struct APIMAPMODEL {
    FILE* file;
    int cursor;
} apiMapModel;

// mode (1-read 2-write)
apiMapModel* fapiOpen(const char* filename);
void fapiClose(apiMapModel* api);

void fapiFree(void* object);

// read
// 1 - not 0 - map 3 - error
int fapiCheckMapSign(apiMapModel* api);

char* fapiGetMapSong(apiMapModel* api);
char* fapiGetMapBGFile(apiMapModel* api);
char* fapiGetMapText(apiMapModel* api, const char* key);

void fapiResetCursor(apiMapModel* api);

// creating file
// 1 - error
int fapiCreateFile(const char* path, const char* audio_file, const char* bg_file, const char* text_content, const char* key);

// encrypt
unsigned char* fapiEncrypt(unsigned char* in_data, int in_data_size, const char* key);

#endif
