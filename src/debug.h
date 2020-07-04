#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <iostream>

#define LOGI

#if LOGI 1
#define LOG(X) do{ std::cout << "[LOG]" __FILE__ ":" << __LINE__ << X << std::endl; }while(0)
#define LOGC(format, ...) do{ fprintf(stdout, "[LOG]%s:%d" format "\n", __FILE__, __LINE__, ##__VA_ARGS__); }while(0)
#else
#define LOG(X)
#define LOGC(...)
#endif

#endif //__DEBUG_H__ 