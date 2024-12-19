#include "menu.h"
#include <stdlib.h>
#include <Windows.h>

char** game_menu_get_dir_list(const char* dir, int* dir_count) {
    WIN32_FIND_DATAA win_data;
    HANDLE file = FindFirstFileA(dir, &win_data);
    if (file == INVALID_HANDLE_VALUE) return NULL;

    // count files
    int directory_count = 0;

    do {
        if (win_data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
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
        if (win_data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
            int filename_size = strlen(win_data.cFileName);
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
