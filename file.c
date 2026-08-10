#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "file.h"


const char *readFile(char *path)
{
    char *text = calloc(1, sizeof(char));
    if (text == NULL)
    {
        printf("Malloc failed in readFile");
        return NULL;
    }
    int textLength = 0;

    FILE *pFile = fopen(path, "r");
    char buffer[1024] = {0};
    if (pFile == NULL)
    {
        printf("Could not open file");
        return NULL;
    }
    while (fgets(buffer, sizeof(buffer), pFile) != NULL)
    {
        textLength += strlen(buffer) * sizeof(char);
        char *temp = realloc(text, textLength + 1);
        if (temp == NULL)
        {
            printf("Realloc failed in readFile");
            return NULL;
        }
        text = temp;
        strcat(text, buffer);
    }

    return text;
}