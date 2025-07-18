#include <stdio.h>
#include <stdlib.h>

typedef struct{
  int capacity;
  int arrsize;
  char** value;
} nb_arr;

void nb_init(nb_arr *newarr, int initial_capacity){
    newarr->value = (char**)malloc(sizeof(char*)* initial_capacity);
    newarr->capacity = initial_capacity;
    newarr->arrsize = 0;
}

void nb_alloc(nb_arr* newarr, char** newval){
  if (newarr->arrsize >= newarr->capacity){
    newarr->capacity *=2;
    newarr->value = newval;
  } else {
    newarr->value = newval;
  }
}



