#include"minicrt.h"
#define EOF -1
int
fputc(int c, FILE* stream)
{
    if(fwrite(&c, 1, 1, stream) != 1) {
        return EOF;
    }
    return c;
}
int 
fputs(const char* str, FILE* stream)
{
    int len = strlen(str);
    if(fwrite((void*)str, 1, len, stream) != len) {
        return EOF;
    }
    return len;
}

#ifndef WIN32
#define va_list char*
#define va_start(ap, arg) (ap = (va_list)&arg + sizeof(arg))
#define va_arg(ap, t) (*(t*)((ap += sizeof(t)) - sizeof(t)))
#define va_end(ap) (ap=(va_list)0)
#else
#include <Windows.h>
#endif

int 
vfprintf(FILE* stream, const char* format, va_list arglist)
{
    int translating = 0;
    int ret = 0;
    const char* p = '\0';
    for(p = format; *p != '\0'; ++p) {
        switch (*p)
        {
        case '%':
            if(!translating) {
                translating = 1;
            }else {
                if(fputc('%', stream) < 0) {
                    return EOF;
                }
                ret++;
                translating = 0;
            }
            break;
        case 'd':
            if(translating) {//遇到%d组合
                char buf[16];
                translating = 0;
                itoa(va_arg(arglist, int), buf, 10);
                if(fputs(buf, stream) < 0) {
                    return EOF;
                }
                ret += strlen(buf);
            } else if(fputc('d', stream) < 0) {
                return EOF;
            } else {
                ret++;
            }
            break;

        case 's'://%s
            if(translating) {
                const char* str = va_arg(arglist, const char*);
                translating = 0;
                if(fputs(str, stream) < 0) {
                    return EOF;
                }
                ret += strlen(str);
            } else if(fputc('s', stream) < 0) {
                return EOF;
            } else {
                ret++;
            } 
            break;
        default:
            if(translating) {
                translating = 0;
            }
            if(fputc(*p, stream) < 0) {
                return EOF;
            } else {
                ret++;
            }
            break;
        }
    }
    return ret;
}

//可变参数函数
int
printf(const char* format, ...)
{
    va_list(arglist);
    va_start(arglist, format);
    return vfprintf(stdout, format, arglist);
} 

int
fprintf(FILE* stream, const char* format, ...)
{
    va_list(arglist);
    va_start(arglist, format);
    return vfprintf(stream, format, arglist);
}