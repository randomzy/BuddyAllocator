#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include <cinttypes>
#include <climits>
#include <cstdlib>

template<uint8_t low_order, uint8_t high_order>
class BuddyAllocator
{
public:
    void * balloc(size_t size);
    void bfree(void * ptr, size_t size);
    void bfree(void * ptr);

    BuddyAllocator();
    ~BuddyAllocator();
    BuddyAllocator(BuddyAllocator const &) = delete;
    BuddyAllocator & operator = (BuddyAllocator const &) = delete;

    constexpr static const uint8_t TREE_HEIGHT = high_order - low_order;
    constexpr static const size_t HEAP_SIZE = (size_t)1 << high_order;
    constexpr static const size_t TABLE_SIZE = (size_t)1 << (TREE_HEIGHT - 1 - 3);

    static_assert(low_order < high_order, "minimum block must be smaller than heap size");
    static_assert(high_order < sizeof(void*)*CHAR_WIDTH - 1, "heap order cannot exceed max memory order");
    static_assert(1 << low_order >= 2*sizeof(void*), "minimum block must be able to store 2 addresses");

// private:
    using ptr_t = void *;
    ptr_t heap_begin = nullptr;
    ptr_t free_list[TREE_HEIGHT];
    char split_table[TABLE_SIZE];
    char free_table[TABLE_SIZE];

    bool is_split(int);
    bool is_free(int);
    int index_in_level(ptr_t ptr, int level);

    static int buddy_of(int buddy);
    static int lchild(int parent);
    static int rchild(int parent);
    static int parent(int child);
};

#include "Allocator.inl"

#endif // __ALLOCATOR_H__