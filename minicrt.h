#ifndef __MIN_CRT_H__
#define __MIN_CRT_H__

#ifdef __cplusplus
extern "c" {
  #endif
  
  //malloc.c
  void free(void* ptr);
  void* malloc(unsigned size);
  static int brk(void* end_data_segment);
  int mini_crt_heap_init();

  //字符串
  char* itoa(int n, char* str, int radix);
  int strcmp(const char* src, const char* dst);
  char* strcpy(char* dest, char* src);
  unsigned strlen(const char* str);

  //文件io
  typedef int FILE;
  #ifdef WIN32
  #define stdin  ((FILE*)(GetStdHandle(STD_INPUT_HANDLE)))
  #define stdout ((FILE*)(GetStdHandle(STD_OUTPUT_HANDLE)))
  #define stderr ((FILE*)(GetStdHandle(STD_ERROR_HANDLE)))
  #else
  #define stdin  ((FILE*)0)
  #define stdout ((FILE*)1)
  #define stderr ((FILE*)2)
  #endif

  int mini_crt_io_init();
  FILE* fopen(const char* filename, const char* mode);
  int fread(void* buffer, int size, int count, FILE* stream);
  int fwrite(void* buffer, int size, int count, FILE* stream);
  int fclose(FILE* fp);
  int fseek(FILE* stream, int offset, int set);

  //printf.c
  int fputc(int c, FILE* stream);
  int fputs(const char* str, FILE* stream);
  int printf(const char* format, ...);
  int fprintf(FILE* stream, const char* format, ...);

  //internal
  //void do_global_ctors();
  //void mini_crt_exit_routine();

  //atexit
  //typedef void (*atexit_func_t) (void);
  //int atexit(atexit_func_t func);

  #ifdef __cplusplus
}
#endif

#endif