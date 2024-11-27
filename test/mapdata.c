#include <stdio.h>
#include "apiMap.h"

int main(int argc, char* argv[]) {
    if (argc < 2) return 0;

    apiMapModel* file = fapiOpen(argv[1]);
    if (!file) {
        printf("File not found!\n");
        return 0;
    }

    if (fapiCheckMapSign(file) != 0) {
        printf("File error!\n");
        return 0;
    }

    char* song = fapiGetMapSong(file);
    fapiFree(song);
    char* bg = fapiGetMapBGFile(file);
    fapiFree(bg);
    char* txt = fapiGetMapText(file, "NULL");
    fapiFree(txt);

    unsigned int* timedata = fapiGetMapTimingData(file);
    unsigned int* converted_timedata = fapiConvertTimingData(timedata, sizeof(timedata), sizeof(timedata[0]));

    for (int i = 0; i < sizeof(converted_timedata)/sizeof(converted_timedata[0]); i++) {
        printf("%c", converted_timedata[i]);
    }
    printf("\n");

    fapiFree(timedata);
    fapiFree(converted_timedata);

    return 0;
}
