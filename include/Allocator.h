#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include <cinttypes>
#include <climits>
#include <cstdlib>

template<uint8_t low_order, uint8_t high_order>
class BuddyAllocator
{
public:
    void * allocate(size_t size);
    void deallocate(void * ptr, size_t size);
    void deallocate(void * ptr);

    BuddyAllocator();
    ~BuddyAllocator();
    BuddyAllocator(BuddyAllocator const &) = delete;
    BuddyAllocator & operator = (BuddyAllocator const &) = delete;

    constexpr static uint8_t LEVELS_CNT() { return high_order - low_order + 1; }
    constexpr static size_t HEAP_SIZE() { return (size_t)1 << high_order; }
    constexpr static size_t TABLE_SIZE() { return ((size_t)1 << ((LEVELS_CNT() < 4 ? 4 : LEVELS_CNT()) - 1 - 3)); }

    static_assert(low_order < high_order, "minimum block must be smaller than heap size");
    static_assert(high_order < sizeof(void*)*CHAR_WIDTH - 1, "heap order cannot exceed max memory order");
    static_assert(1 << low_order >= 2*sizeof(void*), "minimum block must be able to store 2 addresses");

// private:
    using ptr_t = void *;
    ptr_t heap_begin = nullptr;
    ptr_t free_list[high_order + 1];
    char split_table[TABLE_SIZE()];
    char free_table[TABLE_SIZE()];

    bool is_split(size_t);
    void toggle_split(size_t);
    bool buddy_free(size_t);
    void toggle_free(size_t);
    size_t index_in_level(ptr_t ptr, int level);
    size_t index_in_tree(ptr_t ptr, int level);

    size_t buddy_of(size_t buddy);
    size_t lchild(size_t parent);
    size_t rchild(size_t parent);
    size_t parent(size_t child);

    void * buddy_address(void *, int);
    struct Links
    {
        void * next;
        void * prev;
    };
    void insert_after(void * &, void *);
    void remove(void *);

//for debugging
    void print_lists();
    void print_free();
    void print_split();
};

#include "Allocator.inl"

#endif // __ALLOCATOR_H__