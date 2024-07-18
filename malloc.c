#include"mincrt.h"

//双向链表
typedef struct _heap_header
{   
    //状态
    //free--used
    enum {
        HEAP_BLOCK_FREE = 0xABABABAB,
        HEAP_BLOCK_USED = 0xCDCDCDCD,
    } type;

    unsigned size;
    //双向链表
    struct _heap_header * prev;
    struct _heap_header * next;
} heap_header;

#define ADDR_ADD(a,o) (((char*)(a)) + o)//?
#define HEADER_SIZE (sizeof(heap_header))

static heap_header* list_head = NULL;//关键，总指针

//调用free，这快内存必须是使用的
void free(void* ptr) {
    heap_header* header = (heap_header*)ADDR_ADD(ptr, -HEADER_SIZE);
    if(header->type != HEAP_BLOCK_USED) {//并没有使用这块内存区
        return;
    }
    header->type = HEAP_BLOCK_FREE;//变为free
    if(header->prev != NULL && header->prev->type != HEAP_BLOCK_USED) {
        //合并这块内存
        header->prev->next = header->next;
        if(header->next != NULL) {
            header->next->prev = header->prev;
        }
        header->prev->size += header->size;
        
        header = header->prev;//与前面的内存合并完成，操作对象转换
    }

    if(header->next != NULL && header->next->type != HEAP_BLOCK_USED) {
        header->size += header->next->size;
        header->next = header->next->next;
    }
}

void* malloc(unsigned size) {
    heap_header* header;
    if(size == 0) {//申请空内存
        return NULL;
    }

    header = list_head;
    while(header != 0) {
        if(header->type == HEAP_BLOCK_USED) {
            header = header->next;
            continue;
        }
        //从一个完整的大内存中分割小内存
        //此刻
        if(header->size > size + HEADER_SIZE && header->size <= size + HEADER_SIZE * 2) {
            header->type = HEAP_BLOCK_USED;
        }

        if(header->size > size + HEADER_SIZE * 2) {
            heap_header* next = (heap_header*)ADDR_ADD(header, size + HEADER_SIZE);
            next->prev = header;
            next->next = header->next;
            next->type = HEAP_BLOCK_USED;
            next->size = header - (size - HEADER_SIZE);
            header->next = next;
            header->size = size + HEADER_SIZE;
            header->type = HEAP_BLOCK_USED;
            return ADDR_ADD(header,HEADER_SIZE);
        }
        header = header->next;
    }
    return NULL;
}

#ifdef WIN32
//linux brk system call
static int brk(void* end_data_segment) {
    //brk system call number : 45
    asm("movl $45, %%eax    \n\t"
        "movl %1, %%ebx     \n\t"
        "int $0x80          \n\t"
        "movl %%eax, $0     \n\t"
        : "=r"(ret) : "m"(end_data_segment) );
}
#endif

#ifdef WIN32
#include<Windows.h>
#endif
int_min_crt_heap_init() {
    void* base = NULL;
    heap_header* header = NULL;
    unsigned size = 1024 * 1024 * 32;//32MBheap
//windows下
#ifdef WIN32
//windos
//对堆进行空间增加
/*
LPVOID VirtualAlloc(
  LPVOID lpAddress,
  DWORD dwSize,
  DWORD flAllocationType,
  DWORD flProtect
);
flAllocationType:
  MEM_COMMIT：在内存或磁盘上的分页文件中为指定的页面区域分配物理存储。尝试提交已提交的页面不会导致函数失败。这意味着可以提交一系列已提交或取消提交的页面，而无需担心失败。
  MEM_RESERVE：保留进程的虚拟地址空间范围，而不分配物理存储。
 保留范围不能由任何其他分配操作（如 malloc 和 LocalAlloc 函数）使用，直到它被释放。保留页可以在对 VirtualAlloc 函数的后续调用中提交。
  MEM_RESET：不支持
  MEM_TOP_DOWN：在尽可能高的地址分配内存。此标志在 Windows Mobile 中被忽略。  
*/
    base = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if(base == NULL) {
        return 0;
    }
#else
//linux
//对堆进行空间增加
    base = (void*)brk(0);
    void* end = ADDR_ADD(base, size);
    end = (void*)brk(end);
    if(!end) {
        return 0;
    }
#endif

    list_head = (heap_header*)base;
    list_head->type = HEAP_BLOCK_FREE;
    list_head->size = size;
    list_head->prev = NULL;
    list_head->next = NULL;
}
