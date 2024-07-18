#include"mincrt.h"
//在windows平台启用
#ifdef WIN32
#include<Windows.h>
#endif

extern int main(int argc, char* argv[]);//声明main函数
void exit(int);//进程退出函数--自实现

//静态函数--报错函数
static void crt_fatal_error(const char* msg) {
    printf("fatal error : %s", msg);
    exit(1);
}
//程序的真正入口函数
void min_crt_entry(void) {
    int ret;
#ifdef WIN32
    //windows下  
    int flag = 0;
    int argc = 0;
    char* argv[16];
    char* cl = GetCommandLineA();//返回命令行参数的函数，windows下的api

    //解析命令行
    //例如“cd /user/my/win”
    argv[0] = cl;
    argc++;
    /*变量定义与假设：
        cl 是一个指向某个字符串（比如命令行参数字符串）的指针。
    flag 是一个标志变量，用于跟踪当前是否在引号内（1 表示在引号内，0 表示不在引号内）。
    argv 是一个指向字符串数组的指针，用于存储解析后的各个参数。
    argc 是一个整数，记录 argv 中当前已存储的参数数量。
    循环解析：
        while(*cl) 循环遍历字符串直到遇到字符串的结束符 '\0'。
    引号处理：
    如果当前字符 *cl 是引号（\"），则切换 flag 的值。如果 flag 是 0（即之前不在引号内），则将其设置为 1（进入引号内）。如果 flag 已经是 1（即已经在引号内），则将其设置为 0（退出引号内）。这确保了引号内的内容被视为一个整体。
    空格与参数分割：
        如果当前字符是空格（' '）且 flag 为 0（即当前不在引号内），则执行以下操作：
    检查 *(cl + 1) 是否非空，确保不是字符串的末尾。这是为了确保分割后的字符串不会指向空字符。
    将 argv[argc] 设置为 cl + 1，即跳过当前空格字符，将 cl 的下一个位置作为新参数的起始位置。
    argc++，增加参数计数。
    将当前空格字符替换为字符串结束符 '\0'，从而在原字符串中创建新的参数边界。
    指针递增：
        cl++，移动到字符串的下一个字符，继续循环。*/
    while(*cl) {
        if(*cl == '\"') {
            if(flag == 0) {
                flag = 1;
            }else {
                flag = 0;
            }
        }else if(*cl == ' ' && flag == 0) {
            if(*(cl + 1)) {
                argv[argc] = cl + 1;
                argc++;
            }
            *cl = '\0';
        }
        cl++;
    }

#else
    //linux下
    int argc;
    char** argv;
    char* ebp_reg = 0;
    //汇编
    //ebp_reg 是C代码中的一个变量，用于接收从 ebp 寄存器读取的值。
    //%0 是一个占位符，用于表示输出操作数（output operand）的第一个项。
    asm ("movl %%ebp, %0 \n":"=r"(ebp_reg));

    /*
        ebp是bound_point--栈帧底指针
        esp是stack_point--栈帧顶指针
    */
   /* struct
    h_a----  |             |
             |             |
             |             |
             |     ....    |
             |0xBF801FDE   |  argv[1]
             |0xBF801FDE   |  argv[0]
             |     2       |  argc
             |     EBP     |
    l_a----  |   .....     |
   */
    argc = *(int*)(ebp_reg + 4);
    argv = (char**)(ebp_reg + 8);
    /*char** base_address = (char**)(ebp_reg + 8);
    for (int i = 0; i < 16; ++i) {
        argv[i] = base_address[i];
    }*/

#endif
    //初始化堆
    if(!mini_crt_heap_init()) {
        crt_fatal_error("heap initialize failed!");
    }
    //初始化io
    if(!mini_crt_io_init()) {
        crt_fatal_error("io initialize failed!");
    }
    
    ret = main(argc, argv);
    exit(ret);
}

//退出函数
void exit(int exitcode) {

#ifdef WIN32
    //调用windows下的api
    ExitProcess(exitcode);
#else  
    asm( "movl  %0, %%ebx \n\t"  // 将exitcode的值移动到寄存器ebx中
     "movl $1, %%eax  \n\t"  // 将立即数1移动到寄存器eax中，表示系统调用号1（用于exit）
     "int  $0x80      \n\t"  // 触发中断0x80，执行系统调用
     "hlt             \n\t"  // 终止处理器执行，通常不会执行到这里，因为进程已经退出
     ::"m"(exitcode));       // 指定输入操作数，"m"表示内存操作数，exitcode是输入值
#endif

}