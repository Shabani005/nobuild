#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int debug;
} nb_opt;

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

typedef struct {
  char** urls;
  char** filenames;
  size_t size;
  size_t capacity;
} nb_downloads;


static nb_downloads nb_default_down;

#define nb_append_da(nb_arr, ...) \
    nb_append_va(nb_arr, \
                       ((const char*[]){__VA_ARGS__}), \
                       (sizeof((const char*[]){__VA_ARGS__})/sizeof(const char*)))


#define nb_qsortsa(arr) nb_qsorts_impl((arr), sizeof(arr)/sizeof(arr[0]))
#define nb_qsortf(arr) nb_qsortf_impl((arr), sizeof(arr)/sizeof(arr[0]))
#define nb_qsorti(arr) nb_qsorti_impl((arr), sizeof(arr)/sizeof(arr[0]))
#define nb_split(string, ...) nb_split_impl(string, (nb_opt) {__VA_ARGS__})

void nb_init(nb_arr *newarr, int initial_capacity); // obsolete

void nb_append(nb_arr *newarr, char *newval);
void nb_append_int(nb_arr *newarr, int myint); // will deprecate soon
void nb_append_float(nb_arr *newarr, float myfloat); // will deprecate soon
void nb_append_va(nb_arr *newarr, const char *items[], int count);

void nb_free(nb_arr *newarr);


char* nb_strdup(const char* s); // make this void that uses realloc later.

void nb_print(nb_arr *newarr);
void nb_print_info(nb_arr *newarr);
void nb_cmd(nb_arr *newarr);

// File utils
void nb_copy_file(char* old_file_name, char* new_file_name);
char* nb_read_file(char* file_name);
void nb_write_file(char* name, char* buf);
nb_file nb_read_file_c(char* file_name);
bool nb_did_file_change(char *filename);
bool nb_does_file_exist(char *filename);
void nb_rebuild(int argc, char **argv);
void nb_mkdir_if_not_exist(char* dirname);
void nb_end();
void include_http_custom(const char* url, const char* filename);
//bool needs_rebuild(); // need to implement rename file first to .old or something like nob does TODO


// Misc utils
int   nb_compf(const void *a, const void *b);
int   nb_compi(const void *a, const void *b);
char* nb_slice_str(char* a, size_t start, size_t end); // python slicing in c :Kappa:
void  nb_qsortf_impl(void *base, size_t nmemb); // these    functions      macros
void  nb_qsorti_impl(void *base, size_t nmemb); //      two          have 
float nb_time();
float nb_sec_to_msec(float sec);

#ifdef NB_IMPLEMENTATION // make sure to define this before using the header
char* nb_slice_str(char* a, size_t start, size_t end){
  size_t len = end-start;
  char* result = malloc(len+1);
  memmove(result, a+start, len);
  result[len] = '\0';
  return result;
}

 
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


void nb_cmd(nb_arr *newarr) {
  #if !defined(__GNUC__) || defined(__clang__)
  fprintf(stderr, "doesnt support windows for now");
  return;
  #endif
    if (newarr->arrsize < 1) {
        printf("USAGE: provide more parameters\n");
        return;
    }

    size_t total_len = 0;
    for (int i = 0; i < newarr->arrsize; i++) {
        total_len += strlen(newarr->value[i]) + 1;
    }

    char *cmd = malloc(total_len + 1 );
    if (!cmd) {
        fprintf(stderr, "Allocation failed in nb_cmd\n");
        return;
    }

    cmd[0] = '\0';
    for (int i = 0; i < newarr->arrsize; i++) {
        strcat(cmd, newarr->value[i]);
        if (i < newarr->arrsize - 1) strcat(cmd, " ");
    }

    printf("[CMD] %s\n", cmd);
    int ret = system(cmd);
    if (ret == -1) perror("system");

    free(cmd);
    nb_free(newarr); 
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


void nb_write_file(char* name, char* buf){ // old name shouldnt be nobuild.c. it should be the name of the current file. 
  nb_file new_file;

  new_file.filep = fopen(name, "wb");
  fwrite(buf, 1, strlen(buf), new_file.filep);
  fclose(new_file.filep);
  // printf("Current buf size: %zu\n", strlen(buf));
}


void nb_copy_file(char* old_file_name, char* new_file_name){ // old name shouldnt be nobuild.c. it should be the name of the current file.
  nb_file old_file; 
  nb_file new_file;

  if (!nb_does_file_exist){
    printf("%s does not exit", old_file_name);
    return;
  }
  
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

bool nb_did_file_change(char *filename){
  struct stat file_old;
  stat(filename, &file_old);

  if (!nb_does_file_exist){
    printf("%s does not exist\n", filename);
    return 0;
  }
  
  struct stat file_new;
  char buf[64];
  sprintf(buf, "%s.old", filename);
  stat(buf, &file_new);

  return difftime(file_old.st_mtime, file_new.st_mtime) > 0;
}


bool nb_does_file_exist(char *filename){
    if (access(filename, F_OK) == 0){
    return true;
  } else {
  return false;
  }
}

void nb_rebuild(int argc, char **argv){
  char *filename = "builder.c";
  char cloned_file[128];
  sprintf(cloned_file, "%s.old", filename); 

  if (nb_does_file_exist(cloned_file)){
    // printf("%s does exist\n", cloned_file);
    if (nb_did_file_change(filename)){
      printf("[Rebuilding]\n");
      nb_copy_file(filename, cloned_file);

      nb_arr cmd;
      char fname[128];

      nb_init(&cmd, sizeof(fname)*2); 
      strncpy(fname, filename, sizeof(fname));
      fname[sizeof(fname)-1] = '\0';
      char *dot = strrchr(fname, '.');
      if (dot != NULL) {
        *dot = '\0';
      }      
      nb_append(&cmd, "gcc");
      nb_append(&cmd, "-o");
      nb_append(&cmd, fname);
      nb_append(&cmd, filename);
      // nb_print_info(&cmd);
      nb_cmd(&cmd);

      printf("[INFO] rebuilt %s\n", filename);
      nb_free(&cmd);
      // printf("[INFO] %s", argv)

      printf("\n");

      for (int i=0; i<argc; ++i){
        nb_append_da(&cmd, argv[i]);
      }
      nb_cmd(&cmd);
            exit(1);

  } else {
    // printf("file did not change\n");
    }
  }else{
    // printf("created %s.old\n", filename);
    nb_copy_file(filename, cloned_file);
  }
}


nb_file nb_read_file_c(char* file_name){ 
  nb_file file; 

  file.filep = fopen(file_name, "rb");
  fseek(file.filep, 0, SEEK_END);
  
  file.filesize = ftell(file.filep);
  file.buf = (char*)malloc(file.filesize+1);
  fseek(file.filep, 0, SEEK_SET);
  fread(file.buf, 1, file.filesize, file.filep);
  fclose(file.filep);
  file.buf[file.filesize] = '\0';
  return file;
}


char* nb_read_file(char* file_name){
  nb_file file; 

  file.filep = fopen(file_name, "r");
  fseek(file.filep, 0, SEEK_END);
  
  file.filesize = ftell(file.filep);
  file.buf = (char*)malloc(file.filesize+1);
  fseek(file.filep, 0, SEEK_SET);
  fread(file.buf, 1, file.filesize, file.filep);
  file.buf[file.filesize] = '\0'; // null termination
  fclose(file.filep);
  return file.buf;
}

void nb_append_va(nb_arr *newarr, const char *items[], int count) {
    for (int i = 0; i < count; i++) {
        nb_append(newarr, (char*)items[i]);
    }
}

int nb_compf(const void *a, const void *b){
  float fa = *(const float*)a;
  float fb = *(const float*)b;
  if (fa < fb) return -1;
  else if (fa > fb) return 1;
  else return 0;
}

int nb_compi(const void *a, const void *b){
  float ia = *(const int*)a;
  float ib = *(const int*)b;
  if (ia < ib) return -1;
  else if (ia > ib) return 1;
  else return 0;
}

int nb_compsa(const void *a, const void *b) {
    const char *sa = *(const char **)a;
    const char *sb = *(const char **)b;

    size_t la = strlen(sa);
    size_t lb = strlen(sb);

    if (la < lb) return -1;
    else if (la > lb) return 1;
    else return 0;
}

void nb_qsortf_impl(void *base, size_t nmemb){ 
  qsort(base, nmemb, sizeof(float), nb_compf);
}

void nb_qsortsa_impl(void *base, size_t nmemb){ 
  qsort(base, nmemb, sizeof(char*), nb_compsa);
}

void nb_qsorti_impl(void *base, size_t nmemb){ 
  qsort(base, nmemb, sizeof(int), nb_compi);
}

char** nb_split_impl(char* string, nb_opt opt){
  size_t n = strlen(string);
  char** split = malloc(sizeof(char*)*n);
  for (int i=0; i<n; ++i){
    split[i] = malloc(2);
    split[i][0] = string[i];
    split[i][1] = '\0';
  }
  split[n] = NULL;
  
  if (opt.debug){
    printf("[");
    for (int i=0; i<n; ++i){
      printf("%s,", split[i]);
    }
    printf("]\n");
  }
  return split;
}

void include_http_custom(const char* url, const char* filename){ // this function is for builder not regular c file.
  nb_arr cmd = {0};
  if (nb_default_down.capacity == 0) {
    nb_default_down.capacity = 256;
    nb_default_down.size     = 0;
    nb_default_down.filenames = malloc(sizeof(char*) * nb_default_down.capacity);
    nb_default_down.urls      = malloc(sizeof(char*) * nb_default_down.capacity);
  }
  if (nb_default_down.size >= nb_default_down.capacity) {
    nb_default_down.capacity*=2;
    nb_default_down.filenames = realloc(nb_default_down.filenames, nb_default_down.capacity);
    nb_default_down.urls      = realloc(nb_default_down.urls, nb_default_down.capacity);
  }
  nb_default_down.urls[nb_default_down.size]      = (char*)url;
  nb_default_down.filenames[nb_default_down.size] = (char*)filename;
  nb_default_down.size++;
  nb_append_da(&cmd, "wget", "-q", "-O", filename, url); // TODO: use libcurl or implement own http thingy
  nb_cmd(&cmd);
}

void nb_end(){
  for (size_t i=0; i<nb_default_down.size; ++i){
      // printf("debug\n");
      if (!remove(nb_default_down.filenames[i])) exit(-1);
      // printf("removed file: %s\n", nb_default_down.filenames[i]);
  }
}

float nb_time(){
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  return t.tv_sec + t.tv_nsec / 1e9;
}

float nb_sec_to_msec(float sec){
  return sec*1000;
}

void nb_mkdir_if_not_exist(char* dirname){
  #ifdef _WIN32
  fprintf(stderr, "not implemented");
  return;
  #endif

  nb_arr cmd = {0};
  nb_append_da(&cmd, "mkdir", "-p", dirname);
  nb_cmd(&cmd);
}
#endif //NB_IMPLEMENTATION

// TODO: add #ifdef NB_STRIP_PREFIX in the future 
