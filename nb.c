#include "nb.h"
  
int main(){
 
  nb_arr cmd;

  nb_init(&cmd, 100);
  nb_alloc(&cmd, "gcc");
  nb_alloc(&cmd, "hello.c");
  nb_alloc(&cmd, "-o test");
  nb_cmd(&cmd);
  return 0;
}

