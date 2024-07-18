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

        if()
    }

}