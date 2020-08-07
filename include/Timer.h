#include <chrono>
#include <cinttypes>

#define MAX_NAME_LEN

// return time in microseconds
static 
inline 
int64_t getutime()
{
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    int64_t micros = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    return micros;
}

// returns time in milliseconds
static
inline
int64_t getmtime()
{
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    int64_t micros = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return micros;
}

struct ProfileResult
{
    int64_t * time_end;
    int64_t * time_start;
    char * name;
};

class TimerProfiler
{
public:
    void beginProfiling();
    void endProfiling();
    void writeProfile();
    static TimerProfiler & getInstance() {
        static TimerProfiler instance; 
        return instance;
    };
private:
    TimerProfiler();
    void writeHeader();
    void writeFooter();
};

class Timer
{
    Timer(){ start = getutime(); }
    ~Timer(){ int64_t elapsed = getutime() - start;};
private:
    int64_t start;
    char name[MAX_NAME_LEN];
};