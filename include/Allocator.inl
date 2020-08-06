#include <cstring>
#include <iostream>
#include <iomanip>
#include <cassert>

template<uint8_t lo, uint8_t hi>
BuddyAllocator<lo,hi>::BuddyAllocator()
    : heap_begin(malloc(HEAP_SIZE()))
{
    if (!heap_begin) {
        //update errorCode
    } else {
        std::cout << HEAP_SIZE() << std::endl << TABLE_SIZE() << std::endl << LEVELS_CNT() << std::endl;
        memset(heap_begin,0,HEAP_SIZE());
        memset(free_table,0,sizeof(free_table));
        memset(split_table,0,sizeof(split_table));
        memset(busy_list,0,sizeof(busy_list));
        insert_after(busy_list[hi], heap_begin);
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
void * BuddyAllocator<lo,hi>::allocate(size_t _size)
{
    if (_size > HEAP_SIZE())
        return nullptr;
    uint8_t k = log2_ceil(_size);
    k = (k > lo ? k : lo);
    uint8_t order = k;
    //fetch free address from free lists
    while(!busy_list[k]) {
        k++;
        if (k > hi)
            return nullptr;
    }
    ptr_t block = busy_list[k];
    remove(block);
    size_t block_id = index_in_tree(block, hi - k);
    while(k-- > order) {
        toggle_split(block_id);
        toggle_free(block_id);
        void * buddy_addr = buddy_address(block, k); //(void*)(uintptr_t(block) + ((uintptr_t)1 << k))
        block_id = index_in_tree(block, hi - k); //lchild(block_id);
        insert_after(busy_list[k], buddy_addr);
    }
    toggle_free(block_id);
    return block;
}

// void deallocate_rec(void * block, size_t block_id)
// {
//     toggle_free(block_id);
//     if (pair_free(block_id) || block_id == 0) {
//         insert(block);
//         return;
//     } else {
//         buddy_addr = find_buddy(block, block_id);
//         unlink(buddy_addr);
//         if (buddy_addr < block)
//             block = buddy_addr;
//         block_id = parent(block_id);
//         toggle_split(parent(block_id));
//         deallocate_rec(block, parent(block_id));
//     }
// }

template<uint8_t lo, uint8_t hi>
void BuddyAllocator<lo,hi>::deallocate(void * ptr)
{
    if ((uintptr_t)ptr < (uintptr_t)heap_begin or uintptr_t(ptr) > (uintptr_t)heap_begin + HEAP_SIZE())
        return;
    int n = hi;
    int block_id;
    //find size of block (and block_id)
    while (n > 0) {
        block_id = index_in_tree(ptr,hi-n);
        if (!is_split(block_id))
            break;
        --n;
    }
    void * block = ptr;
    int i = n;
    for (; i < hi; i++) {
        toggle_free(block_id);
        if (pair_free(block_id))
            break;
        void * buddy_of_block = buddy_address(block, i);
        remove(buddy_of_block);
        if (buddy_of_block < block ) {
            block = buddy_of_block;
        }
        block_id = parent(block_id);
        toggle_split(block_id);
    }
    insert_after(busy_list[i], block);
}

template<uint8_t lo, uint8_t hi>
bool BuddyAllocator<lo,hi>::is_split(size_t bit_id)
{
    return split_table[bit_id/8] & (1 << (bit_id%8));
}
template<uint8_t lo, uint8_t hi>
void BuddyAllocator<lo,hi>::toggle_split(size_t bit_id)
{
    split_table[bit_id/8] ^= (1 << (bit_id%8));
}

template<uint8_t lo, uint8_t hi>
bool BuddyAllocator<lo,hi>::pair_free(size_t buddy_id)
{
    const size_t bit_id = (buddy_id + 1) >> 1;
    return free_table[bit_id>>3] & (1 << (bit_id%8));
}
template<uint8_t lo, uint8_t hi>
void BuddyAllocator<lo,hi>::toggle_free(size_t buddy_id)
{
    const size_t bit_id = (buddy_id + 1) >> 1;
    free_table[bit_id >> 3] ^= (1 << (bit_id%8));
}

template<uint8_t lo, uint8_t hi>
size_t BuddyAllocator<lo,hi>::index_in_level(ptr_t ptr, int level)
{
    return ( (uintptr_t)ptr - (uintptr_t)heap_begin )/( HEAP_SIZE() >> level );
}

template<uint8_t lo, uint8_t hi>
size_t BuddyAllocator<lo,hi>::index_in_tree(ptr_t ptr, int level)
{
    return index_in_level(ptr, level) + (1 << level) - 1;
}

template<uint8_t lo, uint8_t hi>
size_t BuddyAllocator<lo,hi>::buddy_of(size_t buddy)
{
    return buddy + ((buddy & 1) << 1) - 1;
}

template<uint8_t lo, uint8_t hi>
size_t BuddyAllocator<lo,hi>::parent(size_t child)
{
    return (child - 1) >> 1;
}
template<uint8_t lo, uint8_t hi>
size_t BuddyAllocator<lo,hi>::lchild(size_t parent)
{
    return (parent << 1) + 1;
}
template<uint8_t lo, uint8_t hi>
size_t BuddyAllocator<lo,hi>::rchild(size_t parent)
{
    return (parent << 1) + 2;
}
template<uint8_t lo, uint8_t hi>
void * BuddyAllocator<lo,hi>::buddy_address(void * buddy, int height)
{
    uintptr_t relative_addr = (uintptr_t)(buddy) - (uintptr_t)(heap_begin);
    uintptr_t relative_buddy_addr = relative_addr ^ ((uintptr_t)1 << height);
    return (void*)((uintptr_t)heap_begin + relative_buddy_addr);
}
template<uint8_t lo, uint8_t hi>
void BuddyAllocator<lo,hi>::insert_after(void * & after_me , void * new_elem)
{
    Links * new_first = static_cast<Links*>(new_elem);
    Links * old_first = static_cast<Links*>(after_me);
    after_me = new_elem;
    new_first->prev = &after_me;
    if (old_first)
        old_first->prev = new_elem;
    new_first->next = old_first;
}
template<uint8_t lo, uint8_t hi>
void BuddyAllocator<lo,hi>::remove(void * node)
{
    //from left to right predecessor<->current<->successor
    Links * current = static_cast<Links*>(node);
    Links * pred = static_cast<Links*>(current->prev);
    Links * succ = static_cast<Links*>(current->next);
    pred->next = succ;
    if (succ) {
        succ->prev = pred;
    }
}

template<uint8_t lo, uint8_t hi>
void BuddyAllocator<lo,hi>::print_lists()
{
    for (int i = 0; i < hi; i++) {
        std::cout << "free_list[" << std::dec << i << "]: ";
        Links * current = static_cast<Links*>(busy_list[i]);
        while (current != nullptr) {
            std::cout << std::hex << (uintptr_t)current << " ";
            current = static_cast<Links*>(current->next);
        }
        std::cout << std::endl;
    }
    std::cout << std::dec;
}

template<uint8_t lo, uint8_t hi>
void BuddyAllocator<lo,hi>::print_free()
{
    for (uint8_t i = 0; i < TABLE_SIZE(); i++) {
        for (int j = 0; j < 8; j++) {
            std::cout << "id " << i*8 + j << " " << int((free_table[i] & (1 << j)) >> j) << std::endl;
        }
    }
}

template<uint8_t lo, uint8_t hi>
void BuddyAllocator<lo,hi>::print_split()
{
    for (uint8_t i = 0; i < TABLE_SIZE(); i++) {
        for (int j = 0; j < 8; j++) {
            std::cout << "block " << i*8 + j << " " << int((split_table[i] & (1 << j)) >> j) << std::endl;
        }
    }
}