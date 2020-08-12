#include "Allocator.h"
#include <cassert>
#include <random>
#include "Timer.h"
#include <stdexcept>

template<uint8_t lo, uint8_t hi>
void test_toggles()
{
    BuddyAllocator<lo,hi> allocator;
    size_t MAX_ID = (1 << allocator.LEVELS_CNT) - 2;
    for (size_t i = 1; i <= MAX_ID; i++) {
        //pair free must be initialized to all 0
        assert(allocator.pair_free(i) == 0);
    }
    for (size_t i = 1; i <= MAX_ID; i++) {
        allocator.toggle_free(i);
    }
    allocator.print_free();
    for (size_t i = 1; i <= MAX_ID; i++) {
        //when two buddies A and B are toggled, A^B=0
        assert(allocator.pair_free(i) == 0);
    }
    size_t MAX_SPLIT_ID = (1 << (allocator.LEVELS_CNT - 1)) - 2;
    for (size_t i = 0; i < MAX_SPLIT_ID; i++) {
        //is_split must be initialized to all 0
        assert(allocator.is_split(i) == 0);
        allocator.toggle_split(i);
    }
    for (size_t i = 0; i < MAX_SPLIT_ID; i++) {
        assert(allocator.is_split(i) == 1);
        allocator.toggle_split(i);
        assert(allocator.is_split(i) == 0);
    }
}

void allocate4_deallocate4() 
{
    BuddyAllocator<8,10> allocator;
    void * p1 = allocator.allocate(10);
    void * p2 = allocator.allocate(10);
    void * p3 = allocator.allocate(10);
    void * p4 = allocator.allocate(10);
    void * p5 = allocator.allocate(10);
    assert(p5 == nullptr);
    allocator.deallocate(p1);
    allocator.deallocate(p2);
    allocator.deallocate(p3);
    allocator.deallocate(p4);
    void * heap = allocator.allocate(10);
    assert(heap != nullptr);
    assert(heap == allocator.getHeap());
}

template<uint8_t lo, uint8_t hi>
void allocate_all()
{
    BuddyAllocator<lo,hi> allocator;
    int i = 0;
    while(allocator.allocate(lo)) {
        i++;
    }
    assert(i == (1 << (hi-lo)));
    assert(allocator.maxAllocatable() == 0);
    assert(allocator.totalFree() == 0);
    assert(allocator.totalAllocated() == allocator.HEAP_SIZE);
}

template<uint8_t lo, uint8_t hi>
void random_allocations()
{
    BuddyAllocator<lo,hi> allocator;
    int seed = 0;
    std::mt19937 gen(seed);
    std::vector<void *> ptrs;
    std::uniform_int_distribution<> distrib(8,15);
    void * ptr = nullptr;
    while(true) {
        int size = distrib(gen);
        ptr = allocator.allocate(1<<size);
        if (ptr == nullptr) break;
        ptrs.push_back(ptr);
    }
}

int main()
{
    test_toggles<10,15>();
    allocate4_deallocate4();
    allocate_all<8,15>();
    random_allocations<8,25>();
    return 0;
}
