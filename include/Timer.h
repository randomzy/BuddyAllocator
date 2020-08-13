#ifndef __TIMER_H__
#define __TIMER_H__

#include <chrono>
#include <cinttypes>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>

#define PROFILING 1
#if (PROFILING)
#define PROFILING_START(session_name) TimerProfiler::getInstance().beginProfiling(session_name)
#define PROFILING_END() TimerProfiler::getInstance().endProfiling()
#define PROFILE_SCOPE(name) ScopedTimer timer(name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)
#define PROFILE_EVENT_START(name) TimerProfiler::getInstance().writeEvent({name,EventPhase::BEGIN,getntime()})
#define PROFILE_EVENT_END(name) TimerProfiler::getInstance().writeEvent({name,EventPhase::END,getntime()})
#else
#define PROFILING_START(session_name)
#define PROFILING_END(session_name)
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION()
#endif

#define MAX_DESCRIPTOR_LEN 50

//return time in nanoseconds
static
inline
int64_t getntime()
{
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    int64_t nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    return nanos;
}

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
    int64_t millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return millis;
}

enum class EventPhase {BEGIN = 'B', END = 'E', COMPLETE = 'X'};
struct Event
{
    char const * name;
    EventPhase phase;
    int64_t start;
    int64_t end;
};

class TimerProfiler
{
public:
    TimerProfiler & operator = (TimerProfiler const &) = delete;
    TimerProfiler(TimerProfiler const &) = delete;

    void beginProfiling(std::string const & filename)
    {
        if (!inSession) {
            ofs.open(filename, std::ofstream::trunc);
            writeHeader();
            inSession = true;
        } else {
            std::cout << "[ERROR] Trying to start a new profiling session before closing the old one\n";
        }
    }
    void endProfiling()
    {
        if (inSession) {
            writeFooter();
            ofs.flush();
            ofs.close();
            entries = 0;
            inSession = false;
        }
    }
    void writeEvent(Event const & event)
    {
        char ph = static_cast<char>(event.phase);
        if (inSession) {
            if (entries++ > 0)
                ofs << ',';
            ofs << std::fixed;
            ofs << "{";
            ofs << "\"cat\":\"function\",";
            ofs << "\"pid\":0,";
            ofs << "\"tid\":" << 0 << ",";
            ofs << "\"name\":\"" << event.name << "\",";
            ofs << "\"ph\":\"" << ph << "\",";
            if (event.phase == EventPhase::COMPLETE) {
                ofs << "\"dur\":" << (event.end - event.start)/(double)1000 << ',';
            }
            ofs << "\"ts\":" << event.start/(double)1000;
            ofs << "}\n";

            } else {
                std::cout << std::fixed;
                std::cout << "name: " << event.name
                        << ", event_phase: " << ph 
                        << ", start: " << event.start 
                        << ", end: " << event.end  <<std::endl;
            }
    }
    static TimerProfiler & getInstance() {
        static TimerProfiler instance;
        return instance;
    };
private:
    bool inSession = false;
    int entries = 0;
    std::ofstream ofs;
    TimerProfiler() {};
    ~TimerProfiler() 
    {
        endProfiling();
    };
    void writeHeader()
    {
        ofs << "{\"traceEvents\":[\n";
    }
    void writeFooter()
    {
        ofs << "],\"displayTimeUnit\": \"ns\"}";
    }
};

class ScopedTimer
{
public:
    ScopedTimer(char const * _name = "")
    {
        strncpy(name, _name, MAX_DESCRIPTOR_LEN);
        start = getntime();
    }
    ~ScopedTimer()
    {
        int64_t end = getntime();
        TimerProfiler::getInstance().writeEvent(Event{this->name, EventPhase::COMPLETE, start, end});
    };
private:
    int64_t start;
    char name[MAX_DESCRIPTOR_LEN];
};

#endif // __TIMER_H__