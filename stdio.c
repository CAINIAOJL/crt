#include"minicrt.h"
typedef int FILE;
int 
mini_crt_io_init()
{
    return 1;
}

#ifdef WIN32
#include<Windows.h>
//windows下，柄句为HANDLE
//文件打开函数
FILE* 
fopen(const char* filename, const char* mode) 
{
    HANDLE hfile = 0;
    int access = 0;
    int creation = 0;

    if(strcmp(mode, 'w') == 0) {//write
        access |= GENERIC_WRITE;
        creation |= CREATE_ALWAYS;
    }

    if(strcmp(mode, 'w+') == 0) {//write追加模式
        access |= GENERIC_WRITE | GENERIC_READ;
        creation |= CREATE_ALWAYS;
    }

    if(strcmp(mode, 'r') == 0) {
        access |= GENERIC_READ;
        creation += OPEN_EXISTING;
    }
    
    if(strcmp(mode, 'r+') == 0) {
        access |= GENERIC_WRITE | GENERIC_READ;
        creation |= TRUNCATE_EXISTING;
    }

    hfile = CreationFileA(filename, access, 0, 0, creation, 0, 0);
    if(hfile == INVALID_HANDLE_VALUE) {
        return 0;
    }
    return (FILE*)hfile;//返回文件指针
}

int 
fread(void* buffer, int size, int count, FILE* stream)
{
    int read = 0;
    if(!ReadFile((HANDLE)stream, buffer, size * count, &read, 0)) {
        return 0;
    }
    return read;
}

int
fwrite(const void* buffer, int size, int count, FILE* stream)
{
    int write = 0;
    if(!WRITEFile((HANDLE)stream, buffer, size * count, &write, 0)) {
        return 0;
    }
    return write;
}

int
fclose(FILE* fp)
{
    return CloseHandle((HANDLE)fp);
}

int 
fseek(FILE* fp, int offset, int set)
{
    return SetFilePointer((HANDLE)fp, offset, 0, set);
}

#else

static int
open(const char* pathname, int flags, int mode)
{
    int fd = 0;
    asm("movl $5, %%eax   \n\t"// 将系统调用号5（对应open系统调用）移动到EAX寄存器
        "movl %1, %%ebx   \n\t"// 将pathname的地址移动到EBX寄存器
        "movl %2, %%ecx   \n\t"// 将flags移动到ECX寄存器
        "movl %3, %%edx   \n\t"// 将mode移动到EDX寄存器
        "int $0x80        \n\t"// 触发中断0x80，调用内核中的系统调用
        "movl %%eax, %0   \n\t":// 将系统调用的返回值（文件描述符）从EAX寄存器移动到变量fd中
        "=m"(fd):"m"(pathname),"m"(flags),"m"(mode));// 输入部分，分别对应寄存器EBX、ECX和EDX的值
}

static int
read(int fd, void* buffer, unsigned size) 
{
    int ret = 0;
    asm("movl $3, %%eax   \n\t"
        "movl %1, %%ebx   \n\t"
        "movl %2, %%ecx   \n\t"
        "movl %3, %%edx   \n\t"
        "int $0x80        \n\t"
        "movl %%eax, %0    \n\t":
        "=m"(ret):"m"(fd),"m"(buffer),"m"(size));
    return ret;
}

static int
write(int fd, const void* buffer, unsigned size) 
{
    int ret = 0;
    asm("movl $4, %%eax   \n\t"
        "movl %1, %%ebx   \n\t"
        "movl %2, %%ecx   \n\t"
        "movl %3, %%edx   \n\t"
        "int $0x80         \n\t"
        "movl %%eax, %0    \n\t":
        "=m"(ret):"m"(fd),"m"(buffer),"m"(size));
        return ret;
}

static int
close(int fd) 
{
    int ret = 0;
    asm("movl $6, %%eax   \n\t"
        "movl %1, %%ebx   \n\t"
        "int $0x80         \n\t"
        "movl %%eax, %0    \n\t":
        "=m"(ret):"m"(fd));
        return ret;
}

static int
seek(int fd, int offset, int mode)
{
    int ret = 0;
    asm("movl $19, %%eax    \n\t"
        "movl %1, %%ebx     \n\t"
        "movl %2, %%ecx     \n\t"
        "movl %3, %%edx     \n\t"
        "int $0x80          \n\t"
        "movl %%eax, %0      \n\t":
        "=m"(ret):"m"(fd),"m"(offset),"m"(mode));
        return ret;
}

FILE*
fopen(const char* filename, const char* mode)
{
    int fd = -1;
    int flags = 0;
    int access = 00700;//创建文件的权限

    #define O_RDONLY    00
    #define O_WRONLY    01
    #define O_RDWR      02
    #define O_CREAT     0100
    #define O_TRUNC    01000
    #define O_APPEND   02000

    if(strcmp(mode, "w") == 0) {
        flags |= O_WRONLY | O_CREAT | O_TRUNC;
    }

    if(strcmp(mode, "w+") == 0) {
        flags |= O_RDWR | O_CREAT | O_TRUNC;
    }

    if(strcmp(mode, "r") == 0) {
        flags |= O_RDONLY;
    }

    if(strcmp(mode, "r+") == 0) {
        flags |= O_RDWR | O_CREAT;
    }

    fd = open(filename, flags, access);
    return (FILE*)fd;
}

int 
fread(void* buffer, int size, int count, FILE* stream)
{
    return read((int)stream, buffer, size * count);
}

int
fwrite(void* buffer, int size, int count, FILE* stream)
{
    return write((int)stream, buffer, size * count);
}

int
fclose(FILE* stream)
{
    return close((int)stream);
}

int
fseek(FILE* stream, int offset, int set)
{   
    return seek((int)stream, offset, set);
}

#endif