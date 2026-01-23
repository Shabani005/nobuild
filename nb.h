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
#include <sys/mman.h>
#include <stdint.h>
#include <stdarg.h>

typedef struct {
  int debug;
} nb_opt;

typedef struct{
  size_t capacity;
  size_t arrsize;
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

typedef struct {
  size_t count;
  char** values;
} nb_hexinfo; // TODO: add more metadata to hexinfo

typedef struct {
  uint8_t *base;
  size_t size;
  size_t capacity;
} nb_Arena;

typedef struct{
  char**  value;
  size_t* len;
  size_t  count;
  size_t  capacity;
} nb_ht_Strings;

typedef struct{
  char**    value;
  uint32_t* hash;
  size_t    count;
  size_t    capacity; 
} nb_ht_table;


typedef struct {
  char* buf;
  size_t len;
} nb_xxd_info;


// Defaults
#define NB_AR_SIZE 1024*1024
#define NB_TABLE_SIZE 1 << 21
static nb_Arena *default_arena = NULL;
static nb_downloads nb_default_down;
static nb_hexinfo nb_default_info_h = {.count=0};


// Arena 
nb_Arena *nb_ar_init(size_t cap);
void* nb_ar_alloc_generic(nb_Arena *a, size_t size);
void nb_ar_free_generic(nb_Arena *a);
void nb_ar_reset_generic(nb_Arena *a); 

static void ensure_default_arena(void) {
  if (!default_arena) {
    default_arena = nb_ar_init(NB_AR_SIZE);
  }
}

// Macros
#define nb_ar_alloc(size)  ( ensure_default_arena(), nb_ar_alloc_generic(default_arena, size) )
#define nb_ar_free()       ( ensure_default_arena(), nb_ar_free_generic(default_arena) )
#define nb_ar_reset()      ( ensure_default_arena(), nb_ar_reset_generic(default_arena) )


#define nb_append_da(nb_arr, ...) \
    nb_append_va(nb_arr, \
                       ((const char*[]){__VA_ARGS__}), \
                       (sizeof((const char*[]){__VA_ARGS__})/sizeof(const char*)))

#define nb_qsortsa(arr) nb_qsorts_impl((arr), sizeof(arr)/sizeof(arr[0]))
#define nb_qsortf(arr) nb_qsortf_impl((arr), sizeof(arr)/sizeof(arr[0]))
#define nb_qsorti(arr) nb_qsorti_impl((arr), sizeof(arr)/sizeof(arr[0]))
#define nb_split(string, ...) nb_split_impl(string, (nb_opt) {__VA_ARGS__})
#define nb_hexdump(filename) nb_hexdump_generic(filename, &nb_default_info_h)
#define NB_TMP_BUF_COUNT 20
#define NB_TMP_BUF_SIZE  8192

// "Build" System
void nb_init(nb_arr *newarr, int initial_capacity); // obsolete
void nb_append(nb_arr *newarr, char *newval);
void nb_append_int(nb_arr *newarr, int myint); // will deprecate soon
void nb_append_float(nb_arr *newarr, float myfloat); // will deprecate soon
void nb_append_va(nb_arr *newarr, const char *items[], int count);

void nb_free(nb_arr *newarr);

// String utils
char* nb_strdup(const char* s); // make this void that uses realloc later.
char** nb_split_by_delim(char* str, char delim);
char* nb_append_null(char* buf, size_t len);
char* nb_temp_sprintf(const char *fmt, ...);

void nb_print(nb_arr *newarr);
void nb_print_info(nb_arr *newarr);
void nb_cmd(nb_arr *newarr);
void nb_cmdq(nb_arr *newarr);

// File utils
void nb_copy_file(char* old_file_name, char* new_file_name);
char* nb_read_file(char* file_name);
void nb_write_file(char* name, char* buf);
char* nb_hexdump_generic(char* filename, nb_hexinfo *info);

char* nb_xxd(char* filename, nb_xxd_info *info, char* outname);
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
bool nb_is_int(char* v);
bool nb_is_float(char* v);
bool nb_is_number(char* v);

// Hash Table Utils
uint32_t nb_ht_hash(const char* input, size_t size);
size_t   nb_ht_hash_index(nb_ht_table *t, const char *value);
void     nb_ht_hash_append(nb_ht_table *t, const char *value);
void     nb_ht_string_append(nb_ht_Strings* s, char* value);

#ifdef NB_IMPLEMENTATION // make sure to define this before using the header
char* nb_slice_str(char* a, size_t start, size_t end){
  size_t len = end-start;
  char* result = malloc(len+1);
  memmove(result, a+start, len);
  result[len] = '\0';
  return result;
}




void nb_init(nb_arr *newarr, int initial_capacity){
    newarr->value = (char**)malloc(sizeof(char*) * initial_capacity);
    newarr->capacity = initial_capacity;
    newarr->arrsize = 0;
}


void nb_append(nb_arr *a, char *val) {
    if (a->capacity == 0) {
        a->capacity = 16;
        a->arrsize = 0;
        a->value = malloc(sizeof(char *) * a->capacity);
    }

    if (a->arrsize >= a->capacity) {
        a->capacity *= 2;
        a->value = realloc(a->value,
                            sizeof(char *) * a->capacity);
    }

    a->value[a->arrsize++] = strdup(val);
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


void nb_cmdq(nb_arr *newarr) {
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

    // printf("[CMD] %s\n", cmd);
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

  if (!nb_does_file_exist(old_file_name)){
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

  if (!nb_does_file_exist(filename)){
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

char* nb_hexdump_generic(char* filename, nb_hexinfo *info){  
  if (!nb_does_file_exist(filename)){
    fprintf(stderr, "File: '%s' does not exist\n", filename);
    return NULL;
  }
  
  FILE *f = fopen(filename, "rb");
  fseek(f, 0, SEEK_END);
  size_t fsize = ftell(f);

  unsigned char *buf = malloc(fsize);

  fseek(f, 0, SEEK_SET);  
  fread(buf, 1, sizeof(char)*fsize, f);
  buf[fsize+1] = '\0';

  char *newbuf = (char*)malloc(sizeof(char) * fsize * 3+ 1);
  char *p = newbuf;

  size_t count = 0;

  for (size_t i=0; i+1 < fsize; ++i){
    p += sprintf(p, "%02X ", buf[i]);
    count++;
  }
  info->count = count;
  // printf("count: %zu\n", count);
  *p = '\0';
  return newbuf;

  fclose(f);
}

char** nb_split_by_delim(char* str, char delim){
  size_t len = strlen(str);
  size_t start = 0;
  size_t token_c = 0;
  
  char buf[6400];
  char **full = malloc(sizeof(char*)* len+1);

  for (size_t i=0; i<=len; ++i){
    if (str[i] == delim || str[i] == '\0') {
      size_t token_len = i-start;
      memcpy(buf, str+start, token_len);

      buf[token_len] = '\0';
      start = i+1;
      full[token_c++] = strdup(buf);
    }
  }
  full[token_c] = NULL;
  return full;
}

nb_Arena *nb_ar_init(size_t cap){
  nb_Arena *a = malloc(sizeof(nb_Arena));
  void *arena = mmap(NULL, cap, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  a->capacity = cap;
  a->size = 0;
  a->base = arena;

  return a;
}

void *nb_ar_alloc_generic(nb_Arena *a, size_t size){
  if (a->size + size > a->capacity) { 
    fprintf(stderr, "Writing into out of bounds memory\n");
    abort();
  }
  void* ptr = a->base + a->size;
  a->size += size;
  return ptr;
}

void nb_ar_reset_generic(nb_Arena *a){
  a->size = 0; // We just overwrite old data
}

void nb_ar_free_generic(nb_Arena *a){
  if (a->base){
    munmap(a->base, a->capacity);
    free(a);
  }
}


uint32_t nb_ht_hash(const char *s, size_t len) // Bob Jenkins OAT hash
{
    unsigned char *p = (unsigned char*) s;
    uint32_t h = 0;

    while(len--) {
        h += *p++;
        h += (h << 10);
        h ^= (h >> 6);
    }

    h += (h << 3);
    h ^= (h >> 11);
    h += (h << 15);

    return h;
}

size_t  nb_ht_hash_index(nb_ht_table *t, const char *value) {
    uint32_t hashv = nb_ht_hash(value, strlen(value)) % NB_TABLE_SIZE;
    uint32_t start = hashv; // to detect full table loop

    while (t->value[hashv] != NULL) {
        if (strcmp(t->value[hashv], value) == 0) {
            // Found it
            // printf("Found '%s' at index %u\n", value, hashv);
            return hashv;
        }

        // Collision — probe next index
        hashv = (hashv + 1) % NB_TABLE_SIZE;

        if (hashv == start) {
            fprintf(stderr, "Error: hash table is full\n");
            exit(EXIT_FAILURE);
        }
    }

    // Empty slot found
    // printf("Inserting '%s' at index %u\n", value, hashv);
    t->value[hashv] = strdup(value);
    return hashv;
}

void nb_ht_hash_append(nb_ht_table *t, const char *value) {
    if (t->capacity == 0) {
        t->capacity = NB_TABLE_SIZE;
        t->count = 0;
        t->value = calloc(t->capacity, sizeof(char *));
        t->hash = calloc(t->capacity, sizeof(uint32_t));
    }

    if (t->count >= t->capacity * 0.7) { 
        t->capacity *= 2;
        t->value = realloc(t->value, sizeof(char*) * t->capacity);
        t->hash  = realloc(t->hash, sizeof(uint32_t) * t->capacity);
        memset(t->value + t->count, 0,
               sizeof(char*) * (t->capacity - t->count));
        memset(t->hash + t->count, 0,
               sizeof(uint32_t) * (t->capacity - t->count));
    }

    uint32_t hashv = nb_ht_hash(value, strlen(value)) % t->capacity; 
    uint32_t start = hashv;                       

    while (t->value[hashv] != NULL) {
        if (strcmp(t->value[hashv], value) == 0) {
            // printf("Value '%s' already exists at index %u\n", value, hashv);
            return;
        }

        hashv = (hashv + 1) % t->capacity;

        if (hashv == start) {
            fprintf(stderr, "Error: hash table full\n");
            exit(EXIT_FAILURE);
        }
    }
    t->value[hashv] = strdup(value);
    t->hash[hashv] = hashv;
    t->count++;
    // printf("Inserted '%s' at index %u\n", value, hashv);
}


void nb_ht_string_append(nb_ht_Strings* s, char* value){
  if (s->capacity == 0) {
    s->capacity = 256;
    s->count = 0;
    s->value = malloc(sizeof(char*)*s->capacity);
    s->len = malloc(sizeof(size_t)*s->capacity);
  }
  if (s->count >= s->capacity){ s->capacity +=2;
    s->value = realloc(s->value, sizeof(char*)*s->capacity);
    s->len = realloc(s->len, sizeof(size_t)*s->capacity); 
  }
  s->value[s->count] = value;
  char* buf = strdup(value); 
  s->len[s->count] = strlen(buf); // this may not work
  s->count++;
}

char* nb_xxd(char* filename, nb_xxd_info *info, char* outname){  
  if (!nb_does_file_exist(filename)){
    fprintf(stderr, "File: '%s' does not exist\n", filename);
    return NULL;
  }
  
  FILE *f = fopen(filename, "rb");
  fseek(f, 0, SEEK_END);
  size_t fsize = ftell(f);

  unsigned char *buf = malloc(fsize);

  fseek(f, 0, SEEK_SET);  
  fread(buf, 1, sizeof(char)*fsize, f);
  buf[fsize] = '\0';

  // TODO: unhardcode values

  char *newbuf = (char*)malloc(sizeof(char) * fsize * 20+ 1);
  char *p = newbuf;


  char* otherbuf = (char*)malloc(sizeof(char) * fsize * 30+ 1);
  char* p2 = otherbuf;  

  size_t count = 0; 

  for (size_t i=0; i+1 < fsize; ++i){
    p += sprintf(p, "0x%02x, ", buf[i]);
    count++;
  }
  info->len = count;

  *p = '\0';
  newbuf[count*10] = '\0';

  p2 += sprintf(p2, "char %s[%zu] = {", outname, count);
  p2 += sprintf(p2, newbuf);
  p2 += sprintf(p2, "};\n");

  p2 += sprintf(p2, "int %s_count = %zu;", outname, count);
  
  return otherbuf;
  fclose(f);
}


char* nb_append_null(char* buf, size_t len){
  char *newbuf = malloc(sizeof(char*) * len);
  newbuf = buf;
  newbuf[len] = '\0';
  return newbuf;
}

bool nb_is_int(char* v){
  size_t len = strlen(v);
  for (size_t i=0; i<len; ++i){
    if ((unsigned char)v[i] < '0' || (unsigned char)v[i] > '9') return false;
  }
  return true;
}

bool nb_is_float(char* v){
  size_t len = strlen(v);
  size_t dots = 0;

  if (len==0) return false;
  if (v[0] == '.' || v[len-1] == '.') return false;

  for (size_t i=0; i<len; ++i){
    if (v[i] == '.') {
      dots++;
      if (dots > 1) return false;
      continue;
    }
        if ((unsigned char)v[i] < '0' || (unsigned char)v[i] > '9') return false;
  }
  return true;
}

bool nb_is_number(char* v){
  if (nb_is_int(v) || nb_is_float(v)) return true;
  return false;
}

char* nb_temp_sprintf(const char *fmt, ...) {
    static char bufs[NB_TMP_BUF_COUNT][NB_TMP_BUF_SIZE];
    static size_t idx = 0;

    char *out = bufs[idx++ % NB_TMP_BUF_COUNT];

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(out, NB_TMP_BUF_SIZE, fmt, ap);
    va_end(ap);

    return out;
}
#endif //NB_IMPLEMENTATION

// TODO: add #ifdef NB_STRIP_PREFIX in the future 
