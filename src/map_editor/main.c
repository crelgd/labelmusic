#include "apiMap.h"

#include <stdio.h>
#include <string.h>

void help() {
    printf("\
    /w <filename_in> <filename_out> <key> <audioname> <bgname>\n\
    /r <filename> <key>\n");
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

int main(int argc, char* argv[]) {
    if (argc < 2) {
        help();
        return 0;
    }

    apiMapModel* api = NULL;

    if (strcmp(argv[1], "/r")==0) {
        apiMapModel* api = fapiOpen(argv[2]);
        if (!api) {
            help();
            return;
        }
        char* song = fapiGetMapSong(api);
        char* bg = fapiGetMapBGFile(api);
        char* txt = fapiGetMapText(api, argv[3]);
        printf("song: %s\nbg file: %s\ntext: %s\n", song, bg,txt);
        fapiFree(bg);
        fapiFree(song);
        fapiFree(txt);
    } else if (strcmp(argv[1], "/w")==0) {
        char* file_in = uapiLoadFile(argv[2]);
        if (!file_in) {
            printf("map load error\n");
            return;
        }

        int status = fapiCreateFile(argv[3], argv[5], argv[6],file_in,argv[4]);
        if (status == 1) {
            printf("error\n");
            return;
        }

        fapiFree(file_in);
    } else {
        help();
    }

    if (api) fapiClose(api);
    return 0;
}
