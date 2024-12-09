#include "apiMap.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void help() {
    printf("\
    /w <filename_in> <filename_out> <key> <audioname> <bgname> <filename_in_timedata>\n\
    /r <filename> <key>\n");
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

char* readTimeFile(const char* filename) {
    char* timedata = uapiLoadFile(filename);
    int ec;
    unsigned char** split_timedata = split(timedata, &ec);
    free(timedata);

    return split_timedata;
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

        char* timedata = uapiLoadFile(argv[7]);
        int ec;
        unsigned char** split_timedata = split(timedata, &ec);
        free(timedata);

        unsigned int* data = (unsigned int*)malloc(ec * sizeof(unsigned int));

        for (int i = 0; i < ec; i++) {
            data[i] = (unsigned int)atoi(split_timedata[i]);
            free(split_timedata[i]);
        }
        free(split_timedata);
        
        int status = fapiCreateFile(argv[3], argv[5], argv[6],file_in,argv[4], data, ec);
        if (status == 1) {
            printf("error\n");
            return;
        }

        free(data);

        fapiFree(file_in);
    } else {
        help();
    }

    if (api) fapiClose(api);
    return 0;
}
