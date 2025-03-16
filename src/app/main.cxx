// TaskScheduleDriver.cpp : Defines the entry point for the application.
//

#include <TaskScheduler.h>
#include <exception>
#include <iostream>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>
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

int main(int argc, char** argv)
{
    try {
		LibraryLoader ll("TaskSchedulerLib.dll");
        std::condition_variable cv_wait_for_workers;
        ITaskSchedulerPtr scheduler(ll.CreateScheduler(20,cv_wait_for_workers));

        const uint32_t num_of_tasks = 100;
        
        for(uint32_t i = 0; i < 100; i++)
        {
            scheduler->Schedule([]() { cout << "Task With Priority "<<endl; }, 1, 0.0);
        }
        
        // Sleep for 2 seconds
        // std::this_thread::sleep_for(std::chrono::seconds(2));
        
        std::mutex mtx_wait_for_workers;
        
        std::unique_lock<std::mutex> lock(mtx_wait_for_workers);
        cv_wait_for_workers.wait(lock, [scheduler]() { return !scheduler->IsQueueEmpty(); });
        
		
    }
	catch (exception& e) {
		cout << e.what();
	}
    return 0;
}
