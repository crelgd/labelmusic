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

    int arraY_element;

    unsigned char* timedata = fapiGetMapTimingData(file, &arraY_element);
    if (!timedata) {
        printf("get data error!\n");
        return 1;
    }

    int c = 0;
    for (int i = 0; i < arraY_element; i++) {
        if (c >= 4) {
            printf("\n");
            c = 0;
        }
        printf("%2X ", timedata[i]);
        c++;
    }
    printf("\n");

    unsigned int* converted_timedata = fapiConvertTimingData(timedata, arraY_element);
    if (!converted_timedata) {
        printf("data convert error\n");
        return 1;
    }

    for (int i = 0; i < arraY_element/4; i++) {
        printf("%d\n", converted_timedata[i]);
    }
    printf("\n");

    fapiFree(timedata);
    fapiFree(converted_timedata);

    return 0;
}
