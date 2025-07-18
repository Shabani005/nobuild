#include "nb.h"
#include <stdio.h>
  
int main(){
 
  nb_arr myarr;

  nb_init(&myarr, 100);
  nb_alloc(&myarr, (char**)"hello world");
  printf("%s\n", myarr.value[0]);
  return 0;
}
