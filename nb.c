#include "nb.h"


int main(){
 
  nb_arr myarr;

  nb_init(&myarr, 100);
  nb_alloc(&myarr, "echo");
  nb_alloc(&myarr, "hello");
  nb_alloc(&myarr, "world");
  nb_cmd(&myarr);
  return 0;
}

