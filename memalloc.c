#include <unistd.h>
#include <pthread.h>
#include <string.h>

typedef char ALIGN[16];
union header
{
    struct
    {
        size_t size;
        unsigned is_free;
        union header *next;
    } s;
    ALIGN stub; // memory alignment
};
typedef union header header_t;

header_t *head, *tail;

// global lock
// before every action on memory you have to acquire the lock
// once you are done you have to release the lock

pthread_mutex_t global_malloc_lock;

// find memory blocks that have been released and user's requested size
header_t *get_free_block(size_t size)
{
    header_t *curr = head;
    while (curr)
    {
        if (curr->s.is_free && curr->s.size >= size)
            return curr;
        curr = curr->s.next;
    }
    return NULL;
}

void *malloc(size_t size)
{
    size_t total_size;
    void *block;
    header_t *header;

    if (!size)
        return NULL;

    // only one thread is allocating memory at a time
    pthread_mutex_lock(&global_malloc_lock);
    header = get_free_block(size);

    if (header)
    {
        header->s.is_free = 0;
        pthread_mutex_unlock(&global_malloc_lock);
        return (void *)(header + 1);
    }

    total_size = sizeof(header_t) + size;
    // if not found
    // increase heap memory for processes
    block = sbrk(total_size);
    if (block == (void *)-1)
    {
        pthread_mutex_unlock(&global_malloc_lock);
        return NULL;
    }

    // in C , there is no need to cast a void* to any other pointer type
    header = block;
    header->s.size = size;
    header->s.is_free = 0;
    header->s.next = NULL;
    if (!head)
        head = header;
    // if linklist is not empty
    if (tail)
        // the new allocated memory block is linked to the end of the linklist
        tail->s.next = header;
    tail = header;
    pthread_mutex_unlock(&global_malloc_lock);
    return (void *)(header + 1);
}



void free(void *block)
{
    header_t *header, *tmp;
    void *programbreak;

    if (!block)
        return;

    pthread_mutex_lock(&global_malloc_lock);
    header = (header_t *)block - 1;

    // get the current address of program break
    programbreak = sbrk(0);
    // cast to char* type, add the size of the memory block,
    // get the end postion of memory block
    if ((char *)block + header->s.size == programbreak)
    {
        if (head == tail)
        {
            head = tail = NULL;
        }
        else
        {
            tmp = head;
            while (tmp)
            {
                if (tmp->s.next == tail)
                {
                    tmp->s.next = NULL;
                    tail = tmp;
                }
                tmp = tmp->s.next;
            }
        }
        sbrk(0 - sizeof(header_t) - header->s.size);
        pthread_mutex_unlock(&global_malloc_lock);
        return;
    }
    // if the block is not the last one in the linklist
    header->s.is_free = 1;
    pthread_mutex_lock(&global_malloc_lock);
}

void *calloc(size_t num, size_t nsize)
{
    size_t size;
    void *block;
    if (!num || !nsize)
        return NULL;

    size = num * nsize;
    // check mul overflow
    if (nsize != size / num)
        return NULL;

    block = malloc(size);
    if (!block)
        return NULL;

    memset(block, 0, size);
    return block;
}

void *realloc(void *block, size_t size)
{
    header_t *header;
    void *ret;
    if (!block || !size)
        return malloc(size);

    header = (header_t *)block - 1;
    if (header->s.size >= size)
        return block;

    ret = malloc(size);
    if (ret)
    {
        // copy to a new memory block
        memcpy(ret, block, header->s.size);
        // release old memory block
        free(block);
    }
    return ret;
}

/*
1.compile it as a library file
gcc -o memalloc.so -fPIC -shared memalloc.c -lpthread

2.set the enivornment variable LD_PRELOAD to the path of a shared object,
that file will be loaded before any other library
export LD_PRELOAD=$PWD/memalloac.so


*/