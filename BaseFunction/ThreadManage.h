#ifndef THREADMANGE_H
#define THREADMANGE_H
// 线程池
#include "define.h"


// 线程当前的状态
enum ThreadState
{
	THREADSTATE_UNSTART = -1,  // 未开始
	THREADSTATE_TERMINATE = 0, // 终止
	THREADSTATE_PAUSED = 1,    // 暂停
	THREADSTATE_SLEEPING = 2,  // 睡眠
	THREADSTATE_BUSY = 3,      // 忙碌
	THREADSTATE_AWAITING = 4   // 等候
};

// 任务类
// 我们发现使用任务类并不方便，我们还是希望使用函数指针完成该功能
// class DLLS_PORT Task
// {
// public:
// 	virtual ~Task() {};
// 	virtual void Run() = 0;
// };

// 定义的函数类型
typedef int (*Task)(void*);

// 线程类
class DLLS_PORT Thread
{
public:
	Thread();
	Thread(Task task);       // 输入需要写入的工作
	virtual ~Thread();        // 析构函数

	// 对本类的操作
	bool SetNewTask(Task task); // 设置新的任务,只对还未开始的任务有效
	bool ClearTask();            // 清除当前的任务

	bool Start(bool bSuspend = false);       // 启动线程
	bool Pause(int timeout = -1);		 	 // 暂停线程
	bool Resume();							 // 恢复线程
	bool Exit(unsigned long ExitCode);		 // 退出线程
	bool Suspend();							 // 挂起线程

	inline ThreadState CurrentStatus() { return m_threadState; } //　返回当前的工作状态
	
protected:
	HANDLE CreateThread(bool);
	virtual void Run();                     // 执行函数
	HANDLE m_handle;					    // 先前线程的句柄
	unsigned int m_threadID;                // 线程编
	ThreadState m_threadState;              // 当前的线程状态

private:
	static unsigned int WINAPI StaticThreadFunc(void * arg);   // 这是我们内部调用的线程函数

private:
	Task  m_task;              // 本线程需要执行的任务号
};

// 线程池类
// typedef struct DLLS_PORT ThreadID
// {
// 	Thread thread;
// } ST_THREAD_ID, *PST_THREAD_ID;                                          // 对应的线程和对应的id绑定在一起
// 使用模板类的导出工作
//template class DLLS_PORT std::vector<ST_THREAD_ID>;
typedef struct _ST_THD_INFO
{
	void* worker;               // 对应线程的指针
	HANDLE hStart;				// 启动事件
	HANDLE hHasTask;			// 已经接受到任务
	HANDLE hCanExit;			// 允许退出事件
	HANDLE hHasExit;			// 已经退出事件
}ST_THD_INFO, *PST_THD_INFO;
template class DLLS_PORT std::vector<ST_THD_INFO>;
template class DLLS_PORT std::list<Task>;

class DLLS_PORT ThreadManage
{
// 为了只让给类在一个函数中只能生成一个实例
public:
	static ThreadManage* CreateInstance(int minThreadNum,int maxThreadNum);// 外部使用该静态函数创建该类实例
	static void DeleteInstance();						   // 销毁该类实例
	bool Execute(Task task);                               // 使用id，激活对应的线程

private:
	explicit ThreadManage(int minThreadNum, int maxThreadNum);
	virtual ~ThreadManage();
	Task GetTask();

private:
	class Worker :public Thread
	{
	public:
		Worker(ThreadManage* parent, ST_THD_INFO& threadInfo);
		~Worker();

	protected:
		void Run();
		void Exit();

	private:
		ST_THD_INFO m_stThdInfo;
		ThreadManage* m_parent;
	};
	friend Worker;                                           // 需要和内存池是友缘类

	typedef std::list<Task> Tasks;							 // 任务列表
	typedef std::vector<ST_THD_INFO> ThreadPool;		     // 用于存放线程
	typedef ThreadPool::iterator  ThreadPoolItr;             // 用于存放线程

	int m_iTheadNum;                            // 线程当前
	static ThreadManage* m_instance;            // 指向当前的实例的指针
	int m_iMinThreadNum;                        // 最小线程数
	int m_iMaxThreadNum;                        // 最大线程数

	Tasks m_tasks;                              // 我的任务列表
	ThreadPool m_threadPool;                    // 我的线程池
	CRITICAL_SECTION m_csTasksLock;             // 任务列表的临界区
	CRITICAL_SECTION m_csThreadPoolLock;        // 线程池的临界区

	bool m_bRun;                                // 当前线程池是否可用
};

#endif // !BASEFUNCTION_H