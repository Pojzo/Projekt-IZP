#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_LENGTH            102	// velkost buffera pre ukladanie hesla

void PrintSubstrings(char substrings[][BUFFER_LENGTH], unsigned num_substrings) {
    for (int i = 0; i < num_substrings; i++) {
        printf("%s %d\n", substrings[i], strlen(substrings[i]));
    }
}
