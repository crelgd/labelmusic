#include "apiMap.h"

int main(int argc, char* argv[]) {
    if (argc < 2) return 0;

    unsigned int timedata[3] = {300, 300, 300};

    fapiCreateFile(argv[1], "NONE", "NONE", " word1 word2 word3", "KEY", timedata, 3);

    return 0;
}