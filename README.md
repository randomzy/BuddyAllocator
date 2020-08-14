# Buddy Allocator
## Overview
This is a course project for Systems Programming course at FMI, Sofia University. Two version of buddy allocator are implemented - a classic buddy allocator described [here](https://en.wikipedia.org/wiki/Buddy_memory_allocation). Profiling tool is also provided. 

## Required Packages

- GCC 4.4.x or later  
- CMake 3.12 or higher

## Building
``` bash
mkdir build 
cd build
cmake ..
make .  
```

## Resources
[[1] Tianfu Ma, buddy, Github repository](https://github.com/matianfu/buddy)

[[2] David Robins, buddy-allocator, Github repository](https://github.com/dbrobins/buddy-allocator)

[[3] Steven M. Donahue, Matthew P. Hampton, Morgan Deters, Jonathan M. Nye, Ron K. Cytron, and Krishna M. Kavi, Storage Allocation for Real-Time, Embedded Systems](https://csrl.cse.unt.edu/kavi/Research/DARPA-Workshop-01.pdf)

[[4] Niklas, Allocation Adventures 3: The Buddy Allocator](http://bitsquid.blogspot.com/2015/08/allocation-adventures-3-buddy-allocator.html)

[[5] Bruno R. Preiss, Data Structures and Algorithms with Object-Oriented Design Patterns in C++](https://book.huihoo.com/data-structures-and-algorithms-with-object-oriented-design-patterns-in-c++/html/page431.html)

[[6] Arie Kaufman , Tailored-List and Recombination-DelayingBuddy Systems](https://cvc.cs.stonybrook.edu/Publications/1984/Kau84/file.pdf)