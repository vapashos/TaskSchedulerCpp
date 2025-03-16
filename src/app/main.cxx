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
#ifdef _WIN32
#include <Windows.h>
#define LOAD_FUNC_ADDR(moduleHandler, funct)      GetProcAddress(moduleHandler, funct)
#define MODULE_HANDLER                            HMODULE   
#else
#include <dlfcn.h>
#define LOAD_FUNC_ADDR(moduleHandler, funct)      dlsym(moduleHandler, funct)
#define MODULE_HANDLER                            void*
#endif
using namespace std;


class LibraryLoader
{
public:

    using CreateSchedulerFunc = ITaskScheduler* (*)(size_t,std::condition_variable&);


    LibraryLoader(const char* libName)
    {
        #ifdef _WIN32
        hModule_ = LoadLibrary((LPCSTR)libName);
        #else
        hModule_ = dlopen(libName,RTLD_LAZY);
        #endif
        if (!hModule_) {
            throw exception("Failed to load DLL!\n");
        }

		cout << "DLL loaded successfully\n";
    }
    ~LibraryLoader()
    {
        #ifdef _WIN32
        FreeLibrary(hModule_);
        #else
        dlclose(hModule_);
        #endif
    }

    ITaskScheduler* CreateScheduler(size_t threads_nm,std::condition_variable& mainThreadCV)
    {
        if(!CreateScheduler_)
        {
            CreateScheduler_ = (CreateSchedulerFunc)LOAD_FUNC_ADDR(hModule_, "CreateScheduler");
            if(!CreateScheduler_)
                throw exception("Failed to load CreateScheduler function\n");
        }

        return CreateScheduler_(threads_nm,mainThreadCV);            
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
    size_t num_threads{ std::thread::hardware_concurrency() };
    size_t num_tasks{50};
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
		LibraryLoader ll("TaskSchedulerLib.dll");
        std::condition_variable cv_wait_for_workers;
        ITaskSchedulerPtr scheduler(ll.CreateScheduler(num_threads,cv_wait_for_workers));

        for(uint32_t i = 0; i < num_tasks; i++)
        {
            uint32_t priority = getRandomPriority();
            scheduler->Schedule([]() { cout << "Task With Priority "<<endl; }, priority, 0.0);
        }
        
		scheduler->Start();
        std::mutex mtx_wait_for_workers;
        std::unique_lock<std::mutex> lock(mtx_wait_for_workers);
        cv_wait_for_workers.wait(lock, [scheduler]() { return !scheduler->IsQueueEmpty(); });
        
        scheduler->Terminate();
		const LatencyStats* stats = scheduler->GetLatencyStats();
		cout << "Latency Stats: Min: " << stats->min << " Max: " << stats->max << " Avg: " << stats->Avg() << endl;
		
    }
	catch (exception& e) {
		cout << e.what();
	}
    return 0;
}
