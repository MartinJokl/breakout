#pragma once

#include <stdlib.h>

typedef struct {
    unsigned int count;
    unsigned int capacity;
    unsigned int objectSize;
    void *data;
} List;

List *createList(unsigned int capacity, unsigned int objectSize);
void addToList(List *list, void *obj);
void freeList(List *list);