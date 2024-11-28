// MIT License. See more in LICENSE.txt
// Copyright (c) crelgd

#include "apiMap.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SIGN 0x01, 0x54, 0x4D, 0x46, 0x2E, 0x00
#define SIGN_SIZE 6

apiMapModel* fapiOpen(const char* filename) {
    apiMapModel* model = (apiMapModel*)malloc(sizeof(apiMapModel));

    model->file = fopen(filename, "rb");
    model->cursor = 0;

    return model;
}

void fapiClose(apiMapModel* api) {
    fclose(api->file);
    free(api);
}

int fapiCheckMapSign(apiMapModel* api) {
    if (!api->file) return 3; // file not open
    fseek(api->file, 0, SEEK_SET);

    unsigned char cache[SIGN_SIZE] = {0};
    unsigned char right_data[SIGN_SIZE] = {SIGN};

    if (fread(&cache, sizeof(unsigned char), SIGN_SIZE, api->file) != SIGN_SIZE) { // write first file data to cache
        return 3;
    }

    if (memcmp(cache, right_data, 6) == 0) { // if rd == cache - map
        return 0;
    }

    return 1;
}

int get_result_from_two_bytes(unsigned char arr[2]) {
	return ((int)arr[0] << 8) + (int)arr[1];
}
int get_value_from_four_bytes(unsigned char arr[4]) {
	return ((int)arr[0] << 24) |
		((int)arr[1] << 16) |
		((int)arr[2] << 8) |
		((int)arr[3]);
}

char* fapiGetMapSong(apiMapModel* api) {
    if (!api->file) return NULL;
    fseek(api->file, SIGN_SIZE, SEEK_SET);

    unsigned char ccache[2] = {0};
    if (fread(&ccache, sizeof(unsigned char), 2, api->file) != 2) return NULL;

    int symbols_count = get_result_from_two_bytes(ccache);

    char* result = (char*)malloc(symbols_count*sizeof(char));
    if (!result) return NULL;

    if (fread(result, sizeof(char), symbols_count, api->file) != symbols_count) {
        free(result);
        return NULL;
    }

    api->cursor += SIGN_SIZE+2+symbols_count;

    fseek(api->file, 0, SEEK_SET);

    return result;
}

char* fapiGetMapBGFile(apiMapModel* api) {
    if (!api->file) return NULL;
    fseek(api->file, api->cursor, SEEK_SET);

    unsigned char cache[2] = {0};
    if (fread(&cache, sizeof(unsigned char), 2, api->file) != 2) return NULL;

    int symbols_count = get_result_from_two_bytes(cache);

    char* result = (char*)malloc(symbols_count * sizeof(char));
    if (!result) return NULL;

    if (fread(result, sizeof(char), symbols_count, api->file) != symbols_count) {
        free(result);
        return NULL;
    }

    api->cursor += 2+symbols_count;
    fseek(api->file, 0, SEEK_SET);
    return result;
}

void fapiFree(void* object) {
    free(object);
}

//encrypt
int low_symbol(char* symbol) {
    if (isalpha(symbol)) {
        *symbol = tolower(*symbol);
    } else return NULL;
}
char* byte_to_char(unsigned char byte) {
    char* cbyte = (char*)malloc(3*sizeof(char));
    if (!cbyte) return NULL;
    cbyte[2] = "\0";
    sprintf(cbyte, "%02x", byte);
    return cbyte;
}
char* byte_to_bits(unsigned char int_value) {
    char* output = (char*)malloc(9 * sizeof(char));
    if (!output) return NULL;
    memset(output, 0, 9*sizeof(char));

    char* hex_value = byte_to_char(int_value);

    const char* keys[] = {
        "0000", "0001", "0010", "0011", // 0, 1, 2, 3
        "0100", "0101", "0110", "0111", // 4, 5, 6, 7
        "1000", "1001", "1010", "1011", // 8, 9, A, B
        "1100", "1101", "1110", "1111"  // C, D, E, F
    };

    int hex_counter = 0;
    int counter;
    int added_to_out = 0;

    for (int i = 0; i < 2; i++) {
        if (hex_value[hex_counter] == '0') counter = 0;
        else if (hex_value[hex_counter] == '1') counter = 1;
        else if (hex_value[hex_counter] == '2') counter = 2;
        else if (hex_value[hex_counter] == '3') counter = 3;
        else if (hex_value[hex_counter] == '4') counter = 4;
        else if (hex_value[hex_counter] == '5') counter = 5;
        else if (hex_value[hex_counter] == '6') counter = 6;
        else if (hex_value[hex_counter] == '7') counter = 7;
        else if (hex_value[hex_counter] == '8') counter = 8;
        else if (hex_value[hex_counter] == '9') counter = 9;
        else if (hex_value[hex_counter] == 'a') counter = 10;
        else if (hex_value[hex_counter] == 'b') counter = 11;
        else if (hex_value[hex_counter] == 'c') counter = 12;
        else if (hex_value[hex_counter] == 'd') counter = 13;
        else if (hex_value[hex_counter] == 'e') counter = 14;
        else if (hex_value[hex_counter] == 'f') counter = 15;
        else break;

        for (int i = 0; i < 4; i++) {
            output[added_to_out] = keys[counter][i];
            added_to_out++;
            if (added_to_out > 8) {
                break;
            }
        }

        hex_counter++;
    }

    if (strlen(output) < 8) { 
        free(hex_value);
        if (output) free(output);
        return NULL;
    }

    free(hex_value);

    return output;
}
unsigned char xor(char val1, char val2) {
    int res = '!';
    if (val1 == '0' && val2 == '0') res = '0';
    else if (val1 == '0' && val2 == '1') res = '1';
    else if (val1 == '1' && val2 == '0') res = '1';
    else if (val1 == '1' && val2 == '1') res = '0';
    return res;
}

int binaryToInt(char* binaryStr) {
    int result = 0;
    
    for (int i = 0; i < 8; i++) {
        if (binaryStr[i] == '1') {
            result += (1 << (8 - 1 - i));
        }
    }

    return result;
}

// хз че тут написал
// лень обьяснять
unsigned char* fapiEncrypt(unsigned char* in_data, int in_data_size, const char* key) {
    if (!in_data) return NULL;
    if (!key) return NULL;

    int key_size = strlen(key);

    unsigned char* new1_key = (unsigned char*)malloc(in_data_size * sizeof(unsigned char));
    if (!new1_key) return NULL;

    if (key_size > in_data_size) {
        memcpy(new1_key, key, in_data_size);
    }
    else if (key_size <= in_data_size) {
        int ac = 0;
        for (int i = 0; i < in_data_size; i++) {
            new1_key[i] = key[ac];
            ac++;
            if (ac >= key_size) ac = 0;
        }
    }

    int tbs = in_data_size*8;

    char* bit_arr_in = (char*)malloc(tbs * sizeof(char));
    if (!bit_arr_in) {
        free(new1_key);
        return NULL;
    }

    int pos = 0;
    for (int i = 0; i < in_data_size; i++) { // read to bit array
        char* bits1 = byte_to_bits(in_data[i]);
        for (int i1 = 0; i1 < 8; i1++) {
            bit_arr_in[pos] = bits1[i1];
            pos ++;
        }
        free(bits1);
        bits1 = NULL;
    }
    char* bit_arr_key = (char*)calloc(tbs, sizeof(char));
    if (!bit_arr_key) {
        free(bit_arr_in);
        free(new1_key);
        return NULL;
    }
    pos = 0;
    for (int i = 0; i < in_data_size; i++) { // read to bit array
        char* bits1 = byte_to_bits(new1_key[i]);
        for (int i1 = 0; i1 < 8; i1++) {
            bit_arr_key[pos] = bits1[i1];
            pos ++;
        }
        free(bits1);
        bits1 = NULL;
    }

    char* bin_output = (char*)calloc(tbs, sizeof(char));
    if (!bin_output) {
        free(bit_arr_in);
        free(bit_arr_key);
        free(new1_key);
        return NULL;
    }

    for (int i = 0; i < tbs; i++) { // encrypting
        bin_output[i] = xor(bit_arr_in[i], bit_arr_key[i]);
    }

    // clear
    free(bit_arr_in);
    free(bit_arr_key);
    free(new1_key);

    unsigned char* output = (unsigned char*)calloc(in_data_size, sizeof(unsigned char));
    if (!output) return NULL;

    // bin to int
    pos = 0;
    unsigned char bin_cache[8] = {0}; // timely cache for binary values
    for (int i = 0; i < in_data_size; i++) {
        for (int j = 0; j < 8; j++) {
            bin_cache[j] = bin_output[pos];
            pos++;
        }
        output[i] = binaryToInt(bin_cache);
    }
    free(bin_output);
    return output;
}

void fapiResetCursor(apiMapModel* api) {
    api->cursor = 0;
}

char* fapiGetMapText(apiMapModel* api, const char* key) {
    if (api->cursor <= 0) return NULL;
    unsigned char ccache[4] = {0};

    fseek(api->file, api->cursor, SEEK_SET);

    if (fread(ccache, sizeof(unsigned char), 4, api->file) != 4) return NULL;

    int text_size = get_value_from_four_bytes(ccache);

    char* txt = (char*)malloc(text_size * sizeof(char));
    if (!txt) return NULL;

    int reader = fread(txt, sizeof(char), text_size, api->file);

    if (reader != text_size) {
        free(txt);
        return NULL;
    }

    char* txt_output = fapiEncrypt(txt, text_size, key);
    if (!txt_output) {
        free(txt);
        return NULL;
    }
    free(txt);

    fseek(api->file, 0, SEEK_SET);

    api->cursor += 4+text_size;

    return txt_output;
}

unsigned char* fapiGetMapTimingData(apiMapModel* api, int* array_elements) {
    if (!api) return NULL;
    unsigned char size[4] = {0};

    fseek(api->file, api->cursor, SEEK_SET);

    if (fread(size, sizeof(unsigned char), 4, api->file) != 4) return NULL;
    
    int data_size = get_value_from_four_bytes(size);
    *array_elements = data_size;

    unsigned char* data_array = (unsigned char*)malloc(data_size * sizeof(unsigned char));
    if (!data_array) return NULL;

    if (fread(data_array, sizeof(unsigned char), data_size, api->file) != data_size) {
        free(data_array);
        return NULL;
    }

    fseek(api->file, 0, SEEK_SET);

    api->cursor += 4+data_size;

    return data_array;
}

unsigned int* fapiConvertTimingData(unsigned char* data, int array_elements) {
    if (!data) return NULL;

    int array_size = array_elements/4;

    unsigned int* array = (unsigned int*)malloc(array_size * sizeof(unsigned int));
    if (!array) return NULL;

    int counter = 0;
    int array_pos = 0;
    unsigned char cache[4] = {0};

    for (int i = 0; i < array_elements; i++) {
        if (counter >= 4) {
            if (array_pos >= array_size) break;

            int element_result = get_value_from_four_bytes(cache);
            array[array_pos] = element_result;

            array_pos++;
            counter = 0;
        }

        cache[counter] = data[i];
        counter++;
    }

    if (counter == 4 && array_pos < array_size) {
        int element_result = get_value_from_four_bytes(cache);
        array[array_pos] = element_result;
    }

    return array;
}

// 0 - hight 1 - low
unsigned char* two_bytes_get_low_high(int value) {
	unsigned char* arr = (unsigned char*)malloc(2 * sizeof(unsigned char));
	if (!arr) return NULL;

	arr[0] = (value >> 8) & 0xFF;
	arr[1] = value & 0xFF;

	return arr;
}

// 0 - high 3 - low
unsigned char* four_bytes_get_low_high(int value) {
	unsigned char* arr = (unsigned char*)malloc(4 * sizeof(unsigned char));
	if (!arr) return NULL;

	arr[3] = value & 0xFF;
	arr[2] = (value >> 8) & 0xFF;
	arr[1] = (value >> 16) & 0xFF;
	arr[0] = (value >> 24) & 0xFF;

	return arr;
}

int fapiCreateFile(const char* path, const char* audio_file, const char* bg_file, const char* text_content, const char* key, unsigned int* time_data, int timedata_element_count) {
    int audio_size = strlen(audio_file)+1;
    int textc_size = strlen(text_content)+1;
    int bg_size = strlen(bg_file)+1;
    int output_time_data = timedata_element_count * 4;
    int file_size = SIGN_SIZE+4+4+audio_size+textc_size+bg_size+output_time_data+4;

    char* encrypted_text = fapiEncrypt(text_content, textc_size, key);
    if (!encrypted_text) return 1;

    unsigned char* audio_hex = two_bytes_get_low_high(audio_size);
    unsigned char* textc_hex = four_bytes_get_low_high(textc_size);
    unsigned char* bg_hex = two_bytes_get_low_high(bg_size);

    unsigned char* timedata_hex = four_bytes_get_low_high(output_time_data);

    unsigned char* file_mem = (unsigned char*)malloc(file_size * sizeof(unsigned char));
    if (!file_mem) {
        free(audio_hex);
        free(textc_hex);
        free(encrypted_text);
        free(bg_hex);
        return 1;
    }
    int file_cur_pos = 0;

    unsigned char sing_cache[SIGN_SIZE] = {SIGN};

    for (int i = 0; i < SIGN_SIZE; i++) { // write file sign
        file_mem[file_cur_pos] = sing_cache[i];
        file_cur_pos++;
    }

    for(int i = 0; i < (2+audio_size); i++) {
        if (i < 2) {
            file_mem[file_cur_pos] = audio_hex[i];
        } else {
            file_mem[file_cur_pos] = audio_file[i-2];
        }

        file_cur_pos++;
    }
    free(audio_hex);

    for (int i = 0; i < (2+bg_size); i++) {
        if (i < 2) {
            file_mem[file_cur_pos] = bg_hex[i];
        } else {
            file_mem[file_cur_pos] = bg_file[i-2];
        }

        file_cur_pos++;
    }
    free(bg_hex);

    for (int i = 0; i < (4+textc_size); i++) {
        if (i < 4) {
            file_mem[file_cur_pos] = textc_hex[i];
        } else {
            file_mem[file_cur_pos] = encrypted_text[i-4];
        }

        file_cur_pos++;
    }
    free(textc_hex);
    free(encrypted_text);

    for (int i = 0; i < (4+timedata_element_count); i++) {
        if (i < 4) {
            file_mem[file_cur_pos] = timedata_hex[i];
            file_cur_pos++;
        } else {
            unsigned char* timely_timedata = four_bytes_get_low_high(time_data[i-4]);
            for (int j = 0; j < 4; j++) {
                file_mem[file_cur_pos] = timely_timedata[j];
                file_cur_pos++;
            }
            free(timely_timedata);
        }
    }
    free(timedata_hex);

    FILE* file = fopen(path, "wb");

    if (fwrite(file_mem, sizeof(unsigned char), file_size, file) !=  file_size) {
        free(file_mem);
        fclose(file);
        return 1;
    }

    fclose(file);

    free(file_mem);
    return 0;
}
