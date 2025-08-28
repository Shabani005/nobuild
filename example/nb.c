#define NB_IMPLEMENTATION
#include "nb.h"
  
int main(){
 
  nb_arr cmd = {0};

  nb_append_da(&cmd, "gcc", "-o myhello", "hello.c", "-static");
    
  nb_print_info(&cmd);
  nb_cmd(&cmd);
  
  nb_free(&cmd);
  
  nb_append(&cmd, "./myhello");
  nb_print_info(&cmd);
  nb_cmd(&cmd);

  return 0;
}

