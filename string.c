#include"mincrt.h"
/**
 * int n  为要转换为字符的数字
    char *STR  为转换后的指向字符串的指针
    int radix  为转换数字的进制数
 */
char*
itoa(int n, char* str, int radix)
{
    char digit[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char* p = str;
    char* head = str;
    if(!p) {
        return p;
    }
    if(radix < 2 || radix > 36) {
        return p;
    }
    if(radix != 10 && n < 0) {
        return p;
    }
    //处理特殊
    if(n == 0) {
        *p++ = '0';
        *p = '\0';
        return p;
    }
    //十进制，数字为负数
    if(radix == 10 && n < 0) {
        *p++ = '-';
        n = -n;
    }

    while(n) {
        *p++ = digit[n % radix];
        n /= radix;
    }
    *p = '\0';//结尾添'\0'

    for(--p; head < p; ++head, --p) {
        char temp = *head;
        *head = *p;
        *p = temp;
    }
    return str;
}

//比较字符串的大小
int
strcmp(const char* src, const char* dst) 
{
    int ret = 0;
    unsigned char* p1 = (unsigned char*) src;
    unsigned char* p2 = (unsigned char*) dst;
    while(!(ret = *p2 - *p1) && p2) {
        p1++;
        p2++;
    }
    if(ret < 0) {
        ret = -1;
    }else if(ret > 0) {
        ret = 1;
    }
    return ret;
}

char*
strcpy(char* dest, char* src) 
{
    char* ret = dest;
    while(*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return ret;
}

unsigned
strlen(const char* str)
{
    int cnt = 0;
    if(!str) {
        return 0;
    }
    for(; *str != '\0'; str++){
        cnt++;
    }
    return cnt;
}

/*
#ifndef WIN32
#define va_list char*
#define va_start(ap, arg) (ap = (va_list)&arg + sizeof(arg))
#define va_arg(ap, t) (*(t*)((ap += sizeof(t)) - sizeof(t)))
#define va_end(ap) (ap=(va_list)0)
#else
#include <Windows.h>
#endif

int vfprintf(FILE* stream, const char* format, va_list arglist)
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
            } esle if(fputc('d', stream) < 0) {
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
    va_start(arglist, foramt);
    return vfprintf(stream, format, arglist);
}*/
