#include "Allocator.h"
#include <cassert>
#include <random>
#include "Timer.h"

void * profile_malloc(size_t siz)
{
    PROFILE_SCOPE("malloc");
    return malloc(siz);
}

void profile_free(void  * ptr)
{
    PROFILE_SCOPE();
    free(ptr);
}

template<uint8_t lo, uint8_t hi>
void testToggles()
{
    BuddyAllocator<lo,hi> allocator;
    size_t MAX_ID = (1 << allocator.LEVELS_CNT) - 2;
    for (size_t i = 1; i <= MAX_ID; i++) {
        assert(allocator.pair_free(i) == 0);
    }
    for (size_t i = 1; i <= MAX_ID; i++) {
        allocator.toggle_free(i);
    }
    allocator.print_free();
    for (size_t i = 1; i <= MAX_ID; i++) {
        assert(allocator.pair_free(i) == 0);
    }
    allocator.print_free();
}

void test1() 
{
    BuddyAllocator<8,10> allocator;
    void * p1 = allocator.allocate(10);
    void * p2 = allocator.allocate(10);
    void * p3 = allocator.allocate(10);
    void * p4 = allocator.allocate(10);
    void * p5 = allocator.allocate(10);
    assert(p5 == nullptr);
    allocator.print_free();
    allocator.deallocate(p1);
    allocator.deallocate(p2);
    allocator.deallocate(p3);
    allocator.deallocate(p4);
    void * heap = allocator.allocate(10);
    assert(heap != nullptr);
    assert(heap == allocator.heap_begin);
}

void test2()
{
    const uint8_t lo = 8, hi = 25;
    BuddyAllocator<lo,hi> allocator;
    int i = 0;
    while(allocator.allocate(10)) {
        i++;
    }
    assert(i == (1 << (hi-lo)));
}

void test3()
{
    const uint8_t lo = 8, hi = 25;
    BuddyAllocator<lo,hi> allocator;
    int seed = 0;
    std::mt19937 gen(seed);
    std::vector<void *> ptrs;
    std::uniform_int_distribution<> distrib(8,15);
    void * ptr = nullptr;
    while(true) {
        int size = distrib(gen);
        ptr = allocator.allocate(1<<size);
        profile_malloc(1<<size);
        if (ptr == nullptr) break;
        ptrs.push_back(ptr);
        std::cout << "allocated " << (1<<size) << " bytes\n";
    }
    for (auto & ptr: ptrs) {
        PROFILE_SCOPE("deallocate");
        allocator.deallocate(ptr);
    }
}

void testStatic()
{
    BuddyAllocator<5,10> a1;
    BuddyAllocator<4,11> a2;
    assert((BuddyAllocator<4,10>::HEAP_SIZE != (BuddyAllocator<5,11>::HEAP_SIZE)));
}

int main()
{   
    PROFILING_START("time.json");
    // testToggles<5,10>();
    // test1();
    // test2();
    // testStatic();
    test3();
    return 0;
}