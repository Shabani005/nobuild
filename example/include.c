#include <stddef.h>
#define NB_IMPLEMENTATION
#include "nb.h"

int main(){
  const char* url = "https://raw.githubusercontent.com/Shabani005/cml/refs/heads/master/cml.h";
  const char* file = "cml.h";

  include_http_custom(url, file);

  nb_arr cmd = {0};

  nb_append_da(&cmd, "gcc cml.c -o cml -lm");
  nb_cmd(&cmd);
  
  nb_append_da(&cmd, "./cml");
  nb_cmd(&cmd);

  nb_end();
  return 0;
}
