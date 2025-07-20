#include "nb.h"
  
int main(){
 
  nb_arr cmd;

  nb_append(&cmd, "gcc");
  nb_append(&cmd, "hello.c");
  nb_append(&cmd, "-o myhello");
  nb_append(&cmd, "-static");
  
  

  nb_print_info(&cmd);
  nb_cmd(&cmd);
  
  nb_free(&cmd);
  
  nb_append(&cmd, "./myhello");
  nb_print_info(&cmd);
  nb_cmd(&cmd);

  return 0;
}

