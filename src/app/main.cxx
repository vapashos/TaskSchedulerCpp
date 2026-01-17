// TaskScheduleDriver.cpp : Defines the entry point for the application.
//

#include <TaskScheduler.h>
#include <exception>
#include <iostream>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>
#include <random>
#include <climits>
#include <cstring>
#include <iomanip>
#ifdef _WIN32
    #include <Windows.h>
    #define LOAD_FUNC_ADDR(moduleHandler, funct)      GetProcAddress(moduleHandler, funct)
    #define MODULE_HANDLER                            HMODULE
    #define MODULE_NAME "TaskSchedulerLib.dll"
#else
    #include <dlfcn.h>
    #define LOAD_FUNC_ADDR(moduleHandler, funct)      dlsym(moduleHandler, funct)
    #define MODULE_HANDLER                            void*
    #if defined(__APPLE__)
        #define MODULE_NAME "libTaskSchedulerLib.dylib"
    #else
        #define MODULE_NAME "libTaskSchedulerLib.so"
    #endif
#endif
using namespace std;

static void printCurrentTime() {
    // Get the current time as a time_point
    auto now = std::chrono::system_clock::now();

    // Convert to time_t to get calendar time
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    // Convert to local time and format it
    std::cout << "Current Time: " << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S") << std::endl;
}

class LibraryLoader
{
public:

    using CreateSchedulerFunc = ITaskScheduler* (*)(size_t);


    LibraryLoader(const char* libName)
    {
        #ifdef _WIN32
        hModule_ = LoadLibrary((LPCSTR)libName);
        #else
        hModule_ = dlopen(libName,RTLD_LAZY);
        #endif
        if (!hModule_) {
            std::string msg(string("Failed to load")+MODULE_NAME);
            throw runtime_error(msg);
        }

		cout << "Module loaded successfully ";
        printCurrentTime();
    }
    ~LibraryLoader()
    {
        #ifdef _WIN32
        FreeLibrary(hModule_);
        #else
        dlclose(hModule_);
        #endif
    }

    ITaskScheduler* CreateScheduler(size_t threads_nm)
    {
        if(!CreateScheduler_)
        {
            CreateScheduler_ = (CreateSchedulerFunc)LOAD_FUNC_ADDR(hModule_, "CreateScheduler");
            if(!CreateScheduler_)
                throw runtime_error("Failed to load CreateScheduler function\n");
        }

        return CreateScheduler_(threads_nm);            
    }

private:
    CreateSchedulerFunc CreateScheduler_    { nullptr };
    MODULE_HANDLER      hModule_;

};

uint32_t getRandomPriority() {
    
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    uint32_t seed = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());

    std::mt19937 generator(seed);

    std::uniform_int_distribution<uint32_t> distribution(0, UINT_MAX);

    return distribution(generator);
}


int main(int argc, char** argv)
{
    size_t num_threads{ 10 };
    size_t num_tasks{100000};
    // Parse command-line arguments
    for (int i = 1; i < argc; ++i)
    {

        if (!strcmp(argv[i], "-h"))
        {
            std::cout << "Usage: " << argv[0] << " [-h] [-nthreads <num_threads>] [-ntasks <num_tasks>]\n";
            std::cout << "  -h            Show this help message\n";
            std::cout << "  -nthreads     Number of threads (default: system number of threads)\n";
            std::cout << "  -ntasks       Number of tasks (default: 50)\n";
            return -1;
        }
        else if (!strcmp(argv[i], "-nthreads") && i + 1 < argc)
        {
            num_threads = std::stoul(argv[++i]);
        }
        else if (!(strcmp(argv[i],"-ntasks") && i + 1 < argc))
        {
            num_tasks = std::stoul(argv[++i]);
        }
    }

    try {
        std::cout << "Number of threads: " << num_threads << "number of tasks "<<num_tasks<<std::endl;
		LibraryLoader ll(MODULE_NAME);
        std::condition_variable cv_wait_for_workers;
        ITaskSchedulerPtr scheduler(ll.CreateScheduler(num_threads));

        for(uint32_t i = 0; i < num_tasks; i++)
        {
            uint32_t priority = getRandomPriority()%100;
            scheduler->Schedule([]() { cout << "Hello from thread "<<endl; }, priority, 0.0);
        }
        
		const LatencyStats* stats = scheduler->GetLatencyStats();
		cout << "Latency Stats: Min: " << stats->min << " Max: " << stats->max << " Avg: " << stats->Avg() << endl;
		
    }
	catch (exception& e) {
		cout << e.what();
	}
    return 0;
}
