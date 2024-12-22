#include "menu.h"

#include <stdio.h>

int main(int argc, char* argv[]) {
    if (argc < 2) return 0;

    int count;
    char** data = game_menu_get_dir_list(argv[1], &count);

    for (int i = 0; i < count; i++) {
        printf("%s\n", data[i]);
    }

    game_menu_delete_dir_list(data, count);

    return 0;
}
