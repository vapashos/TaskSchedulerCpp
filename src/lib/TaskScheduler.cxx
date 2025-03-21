#include "TaskScheduler.h"
#include <thread>
#include <vector>
#include <functional>
#include <optional>
#include <queue>
#include <memory>
#include <condition_variable>
#include <iostream>
#include <algorithm>


double LatencyStats::Avg() const
{
    return (max - min) / 2.0;
}


struct TaskTimeStats
{
	TaskTimeStats(std::chrono::steady_clock::time_point t1)
		: enqueue_time_(t1)
    {}

    std::chrono::steady_clock::time_point    enqueue_time_;
    std::chrono::steady_clock::time_point    execute_time_;

    double GetDuration() const
    {
		std::chrono::duration<double> diff = execute_time_ - enqueue_time_;
		return diff.count();
    }
};

//////////////////////////////////////////////////////
//! Task
//////////////////////////////////////////////////////
class Task
{
public:
    explicit Task(std::function<void()> func, uint32_t priority,double deadline);
    Task(const Task&) = delete;
	Task& operator=(const Task&) = delete;

    Task(Task&&) = default;
	Task& operator=(Task&&) = default;

    //! Execute the task
    void Execute() const
    {
		time_stats_->execute_time_ = std::chrono::steady_clock::now();
        func_();
    }
    
    //! Get the priority of the task        
    uint32_t GetPriority()  const
    {   return priority_;    }

    //! Get the id of the task
    uint32_t GetId() const
    {   return id_; }

	//! Get the time statistics of the task
	TaskTimeStats* GetTimeStats() const
	{
		return time_stats_.get();
	}

    //! Less operator implementation in order to be used in priority queue
    bool operator<(const Task& rhs) const
    {
        return GetPriority() < rhs.GetPriority();
    }

private:

    std::function<void()>                    func_;
    uint32_t                                 priority_;
    double                                   deadline_{ 0.0 };
    uint32_t                                 id_{ ++count_ };
	std::unique_ptr<TaskTimeStats>  	     time_stats_;    
    static uint32_t                          count_;
};

uint32_t Task::count_ = 0;

// aliases of local usage
using TaskPtr = std::shared_ptr<Task>;


Task::Task(std::function<void()> func, uint32_t priority,double deadline)
    : func_(func)
    , priority_(priority)
	, deadline_(deadline), time_stats_(new TaskTimeStats(std::chrono::steady_clock::now()))
{}

//////////////////////////////////////////////////////
//! ITaskScheduler
//////////////////////////////////////////////////////
ITaskScheduler::ITaskScheduler(size_t threads)
    : threads_(threads)
{}

ITaskScheduler::~ITaskScheduler()
{
    //TODO join running threads and cleanup their resources

}

const LatencyStats* ITaskScheduler::GetLatencyStats() const
{
    return &lstats_;
}


//////////////////////////////////////////////////////
//! TaskScheduler
//! Implements the ITaskScheduler interface
//////////////////////////////////////////////////////
class TaskScheduler : public ITaskScheduler
{
public:
    //! Creates an entity set in a pool        
    //! \param threads number of threads of the thread pool managed by the scheduler
    explicit TaskScheduler(size_t threads,std::condition_variable& mainThreadCV);

    
    //! Destructor
    ~TaskScheduler()
    {
        Terminate();
    }

    virtual void Schedule(void_callback f, uint32_t priority, double deadline) override
    {
        //Add task in the queue
        task_queue_.emplace(f,priority,deadline);
    }

	virtual void Start() override
	{
        running_ = true;
        cv_wait_on_empty_queue_.notify_all();
	}

	virtual void Terminate() override
	{
        force_stop_ = true;
        cv_wait_on_empty_queue_.notify_all();
		for (auto& t : t_pool_)
		{
			if (t.joinable())
    			t.join();
		}
        std::cout<<"TaskScheduler all threads terminated successfully"<<std::endl;
		running_ = false;
	}

    virtual bool IsQueueEmpty() const override
    {
        return !task_queue_.empty();
    }

private:

    using prio_queue = std::priority_queue < Task, std::vector<Task>,
                             std::less<std::vector<Task>::value_type >>;

    std::vector<std::thread> t_pool_;	
    std::condition_variable  cv_wait_on_empty_queue_;
    std::mutex               mtx_cv_wait_on_empty_queue_;
    std::mutex               mtx_access_queue_; 
    std::condition_variable& mainThreadCV_;
	bool                     running_       { false };
    bool                     force_stop_    { false };
    prio_queue               task_queue_;

    void ThreadJob_();
};


TaskScheduler::TaskScheduler(size_t threads,std::condition_variable& mainThreadCV)
    : ITaskScheduler(threads),mainThreadCV_(mainThreadCV)
 {
    for (size_t i = 0; i < threads; i++)
    {
        t_pool_.emplace_back(&TaskScheduler::ThreadJob_, this);
    }
}

void TaskScheduler::ThreadJob_()
{
    // Each thread runs an infinite loop that can terminate under certain contitions
    // In order to serve the execution of tasks that are available in the backlog (queue).
    while (!force_stop_ && running_)
    {
        // Block in case task_queue is empty
        {
            std::unique_lock<std::mutex> lock(mtx_cv_wait_on_empty_queue_);
            cv_wait_on_empty_queue_.wait(lock, [this]() { return !this->task_queue_.empty() || force_stop_; });
        }

        {
            std::lock_guard<std::mutex> lock(mtx_access_queue_);
            if (force_stop_)
                return;
            if (!task_queue_.empty())
            {
                const Task& task = task_queue_.top();
                std::cout << "Thread " << std::this_thread::get_id() << " is executing a task " << task.GetId() << " ";
                task.Execute();
                // Before removing task get its statistics
                TaskTimeStats* stats = task.GetTimeStats();
                double task_wait_for_execution_time = stats->GetDuration();
                lstats_.min = std::min(lstats_.min, task_wait_for_execution_time);
                lstats_.max = std::max(lstats_.max, task_wait_for_execution_time);
                task_queue_.pop();
            }

            if (task_queue_.empty())
                mainThreadCV_.notify_one();
        }
    }
}


extern "C" TASKSCHEDULERLIB_API
ITaskScheduler* CreateScheduler(size_t threads,std::condition_variable& mainThreadCV)
{
    return new TaskScheduler(threads,mainThreadCV);
}