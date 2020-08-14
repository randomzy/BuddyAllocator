#include "Allocator.h"
#include "Timer.h"
#include <iostream>
#include <vector>

BuddyAllocator<8,23> allocator;

void * operator new(size_t size)
{
    PROFILE_FUNCTION();
    void * ptr = allocator.allocate(size);
    PROFILE_EVENT_START(ptr);
    return ptr;
}

void operator delete(void * ptr)
{
    PROFILE_FUNCTION();
    allocator.deallocate(ptr);
    PROFILE_EVENT_END(ptr);
}

int main()
{   
    PROFILING_START("time.json");
    PROFILE_SCOPE("main");
    int init_tries = 15;
    while(!allocator.init() && init_tries > 0) {init_tries--;};
    if (init_tries == 0) {
        //backup code here
        std::cout << "Could not allocate memory for memory allocator" << std::endl;
        return 1;
    }

    for (int i = 0; i < 10; i++) {
        std::vector<int> a;
        a.push_back(10);
        a.resize(100);
    }
    return 0;
}