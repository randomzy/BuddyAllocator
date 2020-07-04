#include <cstring>

template<uint8_t lo, uint8_t hi>
BuddyAllocator<lo,hi>::BuddyAllocator()
    : heap_begin(malloc(HEAP_SIZE))
{
    if (!heap_begin) {
        //update errorCode
    } else {
        memset(heap_begin,0,HEAP_SIZE);
        memset(free_table,0,sizeof(free_table));
        memset(split_table,0,sizeof(split_table));
        memset(free_list,0,sizeof(free_list));
        free_list[TREE_HEIGHT-1] = heap_begin;
    }
}

template<uint8_t lo, uint8_t hi>
BuddyAllocator<lo,hi>::~BuddyAllocator()
{
    free(heap_begin);
}

static
inline
uint8_t log2(size_t n)
{   
    uint8_t result = 0;
    while(n >>= 1)
    {
        ++result;
    }
    return result;
}

static
inline
uint8_t log2_ceil(size_t n)
{
    return log2(n-1) + 1;
}

template<uint8_t lo, uint8_t hi>
void * BuddyAllocator<lo,hi>::balloc(size_t _size)
{
    if (_size > HEAP_SIZE)
        return nullptr;
    uint8_t k = log2_ceil(_size);
}

template<uint8_t lo, uint8_t hi>
int BuddyAllocator<lo,hi>::index_in_level(ptr_t ptr, int level)
{
    return ((uintptr_t)ptr - (uintptr_t)heap_begin) >> (HEAP_SIZE >> level);
}

template<uint8_t lo, uint8_t hi>
int BuddyAllocator<lo,hi>::buddy_of(int buddy)
{
    return buddy + ((buddy & 1) << 1) - 1;
}

template<uint8_t lo, uint8_t hi>
int BuddyAllocator<lo,hi>::parent(int child)
{
    return (child - 1) >> 1;
}
template<uint8_t lo, uint8_t hi>
int BuddyAllocator<lo,hi>::lchild(int parent)
{
    return (parent << 1) + 1;
}
template<uint8_t lo, uint8_t hi>
int BuddyAllocator<lo,hi>::rchild(int parent)
{
    return (parent << 1) + 2;
}