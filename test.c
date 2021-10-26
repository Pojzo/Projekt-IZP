#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#include <string.h>

#define BUFFER_LENGTH 100

typedef unsigned int uint;

unsigned StringLength(const char* string) {
    unsigned length = 0;
    while (*string) {
        ++string;
        ++length;
    }
    return length;
}

void StringCopy(const char *source, char* dest, uint start, uint substring_len)
{
    uint index = 0;
    for (uint i = start; i < start + substring_len && i <= StringLength(source); i++) {
        if (index >= substring_len) {
            return;
        }
        dest[index++] = source[i];
    }
    dest[index] = '\0';
}

void GetSubstrings(const char *password, uint password_len,
        char substrings[][BUFFER_LENGTH], uint substring_len)
{

    for (uint i = 0; i < password_len - substring_len + 1; i++) {
        StringCopy(password, substrings[i], i, substring_len);
    }
}

int main(int argc, char** argv) {
    const char* string = "Hello432o4iu";
    uint substring_len = 2;
    uint num_substrings = StringLength(string) + 1 - substring_len;
    char substrings[num_substrings][BUFFER_LENGTH];
    GetSubstrings(string, StringLength(string), substrings, substring_len);
/*
//    GetSubstrings(string, substring_len, substrings, substring_len);
    uint index;
    for (uint i = 0; i < num_substrings; i++) {
        index = 0;
        for (uint start = i; start < start + substring_len && start <= StringLength(string); start++) {
            if (index >= substring_len) {
                break;
            }
            substrings[i][index++] = string[start];
        }
    }

    */
    for (uint i = 0; i < num_substrings; i++) {
        printf("%d. %s\n", i + 1, substrings[i]);
    }
    return 0;
}

