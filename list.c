#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "list.h"

List *createList(unsigned int capacity, unsigned int objectSize) {
    List *list = malloc(sizeof(List));
    if (list == NULL)
        return NULL;

    list->capacity = capacity;
    list->objectSize = objectSize;
    list->count = 0;

    list->data = malloc(objectSize * capacity);
    if (list->data == NULL) {
        free(list);
        return NULL;
    }
    return list;
}

void addToList(List *list, void *obj)
{

    if (list->count == list->capacity)
    {
        void *new_data = realloc(list->data, list->objectSize * list->capacity * 2);
        if (new_data == NULL)
            return;

        list->capacity *= 2;
        list->data = new_data;
    }

    memcpy(((uint8_t *)(list->data)) + list->objectSize * list->count++, obj, list->objectSize);
}

void freeList(List *list) {
    if (list == NULL)
        return;
    if (list->data != NULL)
        free(list->data);
    free(list);
}