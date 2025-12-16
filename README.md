
# STB-style Single header utility for C

- Usage (download the file and include it into your project):

```bash
wget https://raw.githubusercontent.com/Shabani005/nobuild/refs/heads/master/nb.h
```

```c
  #define NB_IMPLEMENTATION
  #include "nb.h"
```

# Philosophy
- C is a really old language, but that does not mean it has to feel old doesn't it? this library ports features from more modern languages while preserving the simplicity of C.
 
# Features
- simple **build system** / **recipe builder** (_make_ and _shell_ script replacement)
- simple **hash table** implementation
- simple **arena** implementation for comfortable memory management
- _other_ **misc** utils like split string by delimiters, etc.
