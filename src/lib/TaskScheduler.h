// TaskSchedulerLib.h
#pragma once
#ifdef _WIN32
#ifdef TASKSCHEDULERLIB_EXPORTS
#define TASKSCHEDULERLIB_API __declspec(dllexport)
#else
#define TASKSCHEDULERLIB_API __declspec(dllimport)
#endif
#else
#define TASKSCHEDULERLIB_API	
#endif


#include <cstdint>
#include <memory>

using void_callback = void(*)();

struct TASKSCHEDULERLIB_API LatencyStats
{
    double min{0.0};
    double max{0.0};
    
    //! Returns the average latency
    double Avg() const;
};

//! Class ITaskScheduler interface
class TASKSCHEDULERLIB_API ITaskScheduler
{
public:
    //! Creates an entity set in a pool        
    //! \param threads number of threads of the thread pool managed by the scheduler
    explicit ITaskScheduler(size_t threads);

    //! Joins all running threads. Clean up of thread resources.
    virtual ~ITaskScheduler();

    //! Schedule a new task     
    //! \param f void function pointer
    //! \param priority of the task
	//! \param deadline of the task
    virtual void Schedule(void_callback f, uint32_t priority, double deadline) = 0;

	//! Start the scheduler
	virtual void Start() = 0;

    //! Terminate the scheduler, release all resources
    virtual void Terminate() = 0;

    virtual bool IsQueueEmpty() const = 0;

    //! Get the latency statistics of the scheduler
    const LatencyStats* GetLatencyStats() const;


protected:
    size_t          threads_;
    LatencyStats    lstats_;
};

using ITaskSchedulerPtr = std::shared_ptr<ITaskScheduler>;
