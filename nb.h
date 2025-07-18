#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void nb_alloc(nb_arr* newarr, char* newval){
  if (newarr->arrsize >= newarr->capacity){
    newarr->capacity *=2;
    newarr->value = (char**)realloc(newarr->value, sizeof(char*) * newarr->capacity);
  } 
    newarr->value[newarr->arrsize++] = strdup(newval);
}

void nb_append_int(nb_arr* newarr, int myint){
  char buf[32];
  sprintf(buf, "%d", myint);
  nb_alloc(newarr, buf);
}


void nb_cmd(nb_arr* newarr){
  char* cmd = (char*)malloc(13123232323);
  for (int i; i < newarr->arrsize; i++){
    
    strcat(cmd, strcat(newarr->value[i]," "));
  }
  system(cmd);
}


