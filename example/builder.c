#define NB_IMPLEMENTATION
#include "../nb.h"
  
int main(int argc, char** argv){
  nb_rebuild(argc, argv);
  
  nb_arr cmd = {0};

  nb_append_da(&cmd, "gcc", "-o myhello", "hello.c", "-static");
  nb_cmd(&cmd);
  
  nb_append(&cmd, "./myhello");
  nb_cmd(&cmd);

  return 0;
}

