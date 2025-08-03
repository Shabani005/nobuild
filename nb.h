#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{
  int capacity;
  int arrsize;
  char** value;
} nb_arr;


 typedef struct{
    FILE *filep;
    size_t filesize;
    int chars;
    char *buf;
  } nb_file;

void nb_init(nb_arr *newarr, int initial_capacity);

void nb_append(nb_arr *newarr, char *newval);
void nb_append_int(nb_arr *newarr, int myint);
void nb_append_float(nb_arr *newarr, float myfloat);

void nb_free(nb_arr *newarr);


char* nb_strdup(const char* s); // make this void that uses realloc later.

void nb_print(nb_arr *newarr);
void nb_print_info(nb_arr *newarr);


void nb_cmd(nb_arr *newarr);

// void copy_file(char* old_file_name, char* new_file_name);

void copy_file(char* old_file_name, char* new_file_name);

bool needs_rebuild(); // need to implement rename file first to .old or something like nob does


#ifdef NB_IMPLEMENTATION // make sure to define this before using the header

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



 
/*
  char* nb_strdup(const char* s) {
    char* d = malloc(strlen(s) + 1);
    if (d) strcpy(d, s);
    return d;
}
*/


void nb_init(nb_arr *newarr, int initial_capacity){
    newarr->value = (char**)malloc(sizeof(char*) * initial_capacity);
    newarr->capacity = initial_capacity;
    newarr->arrsize = 0;
}


// later increase cap by size of new sheiSSe
void nb_append(nb_arr *newarr, char *newval){
  if (newarr->value == NULL){
    newarr->capacity =16;
  if (newarr->capacity > 16 | newarr->arrsize > newarr->capacity) {
    newarr->capacity *=2;
  }
    newarr->value = (char**)realloc(newarr->value, sizeof(char*) * newarr->capacity);
  } 
    newarr->value[newarr->arrsize++] = strdup(newval);
}

void nb_append_int(nb_arr *newarr, int myint){
  char buf[64];
  sprintf(buf, "%d", myint);
  nb_append(newarr, buf);
}

void nb_append_float(nb_arr *newarr, float myfloat){
  char buf[64];
  sprintf(buf, "%f", myfloat);
  nb_append(newarr, buf);
}

void nb_print(nb_arr *newarr){
  for (int i = 0; i < newarr->arrsize; i++){
    printf("%s\n", newarr->value[i]);
  }
}

void nb_print_info(nb_arr *newarr){
  printf("[INFO] ");
  for (int i = 0; i < newarr->arrsize; i++){
    printf("%s", newarr->value[i]);
    printf(" ");
  }
  printf("\n");
}

void nb_free(nb_arr *newarr){
  if (newarr->value != NULL){
    for (int i=0; i < newarr->arrsize; i++){
      free(newarr->value[i]);
      newarr->value[i] = NULL;
    }
    free(newarr->value);
    newarr->value = NULL;
  }
  newarr -> capacity = 0;
  newarr -> arrsize = 0;
}


void nb_cmd(nb_arr *newarr){  
  
  if (newarr->arrsize < 1){
    printf("USAGE: provide more parameters\n");
  }

  char* cmd = (char*)malloc(sizeof(char*) *newarr->capacity);
  for (int i=0; i < newarr->arrsize; i++){
    
    strcat(cmd, strcat(newarr->value[i]," "));
  }
  system(cmd);
}


// compile func that requires c_file to run otherwise returns error like <please return usage>
void nb_com(nb_arr *newarr){  
  char* cmd = (char*)malloc(sizeof(char*) *newarr->capacity);
  for (int i=0; i < newarr->arrsize; i++){
    
    strcat(cmd, strcat(newarr->value[i]," "));
  }
  system(cmd);
}


void append_c_file(FILE *filepointer){

}

void copy_file(char* old_file_name, char* new_file_name){ // old name shouldnt be nobuild.c. it should be the name of the current file.
  nb_file old_file; 
  nb_file new_file;

  old_file.filep = fopen(old_file_name, "rb");
  fseek(old_file.filep, 0, SEEK_END);
  
  old_file.filesize = ftell(old_file.filep);
  old_file.buf = (char*)malloc(old_file.filesize);
  fseek(old_file.filep, 0, SEEK_SET);
  fread(old_file.buf, 1, old_file.filesize, old_file.filep);
  fclose(old_file.filep);

  new_file.filep = fopen(new_file_name, "wb");
  fwrite(old_file.buf, 1, old_file.filesize, new_file.filep);
  fclose(new_file.filep);
}




#endif //NB_IMPLEMENTATION


