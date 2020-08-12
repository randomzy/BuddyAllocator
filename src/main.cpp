#include "Allocator.h"
#include "Timer.h"
#include <iostream>
#include <vector>

BuddyAllocator<8,23> allocator;

void * operator new(size_t size)
{
    void * ptr = allocator.allocate(size);
    // void * ptr = malloc(size);
    return ptr;
}

void operator delete(void * ptr)
{
    // ScopedTimer("MyFree");
    allocator.deallocate(ptr); 
    // free(ptr); 
}

template<uint32_t size>
struct AllocateMe
{
    char a[size];
};

int main()
{   
    TimerProfiler::getInstance().beginProfiling("time.json");
    ScopedTimer("main");
    BuddyAllocator<10,15> alloc;
    {
        ScopedTimer("allocInMain");
        alloc.allocate(100);
    }
    int init_tries = 15;
    while(!allocator.init() && init_tries > 0) {init_tries--;};
    if (init_tries == 0) {
        //backup code here
        std::cout << "Could not allocate memory for memory allocator" << std::endl;
        return 1;
    }
    std::vector<int> a;
    for (int i = 0; i < 10; i++) {
        a.push_back(i);
    }

    return 0;
}