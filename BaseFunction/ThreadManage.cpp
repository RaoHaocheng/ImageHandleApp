#include "ThreadManage.h"
#include <process.h>
#include <assert.h>

// 线程
// FIXME::目前线程中的状态机存在问题，需要及时的调整，以免线程池中的代码出现问题
/***************************************************************************
* 函数名称：   Thread
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/08      饶智博        添加
***************************************************************************/
Thread::Thread()
{
	Thread(NULL);
}


/***************************************************************************
* 函数名称：   Thread
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  Task * task
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/08      饶智博        添加
***************************************************************************/
Thread::Thread(Task task)
	:m_task(task),
	m_handle(NULL),
	m_threadState(THREADSTATE_UNSTART),
	m_threadID(-1)
{
}


/***************************************************************************
* 函数名称：   ~Thread
* 摘　　要：   
* 全局影响：   virtual public 
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/08      饶智博        添加
***************************************************************************/
Thread::~Thread()
{

}


/***************************************************************************
* 函数名称：   SetNewTask
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  Task * task
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/08      饶智博        添加
***************************************************************************/
bool Thread::SetNewTask(Task task)
{
	if (NULL != m_handle)
		return false;

	m_task = task;
	return true;
}

/***************************************************************************
* 函数名称：   ClearTask
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/08      饶智博        添加
***************************************************************************/
bool Thread::ClearTask()
{
	if (NULL != m_handle)
		return false;

	m_task = NULL;
	return true;
}

/***************************************************************************
* 函数名称：   Start
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  bool bSuspend
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/08      饶智博        添加
***************************************************************************/
bool Thread::Start(bool bSuspend /* = false */)
{
	if (NULL == m_handle)
		CreateThread(bSuspend);
	else
		Resume();

	return true;
}

/***************************************************************************
* 函数名称：   Pause
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  int timeout
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/08      饶智博        添加
***************************************************************************/
bool Thread::Pause(int timeout)// 暂停线程
{
	if (NULL == m_handle)
		return false;

	if (timeout <= 0)
		timeout = INFINITE;

	m_threadState = ThreadState::THREADSTATE_PAUSED;
	::WaitForSingleObject(m_handle, timeout);
	return true;
}

/***************************************************************************
* 函数名称：   Resume
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/08      饶智博        添加
***************************************************************************/
bool Thread::Resume()							 // 恢复线程
{
	if (NULL == m_handle)
		return false;

	::ResumeThread(m_handle);
	return true;
}
/***************************************************************************
* 函数名称：   Exit
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  unsigned long ExitCode
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/08      饶智博        添加
***************************************************************************/
bool Thread::Exit(unsigned long ExitCode)      // 退出线程
{
	if (NULL == m_handle)
		return false;

	if (::TerminateThread(m_handle, ExitCode))
	{
		::CloseHandle(m_handle);
		m_handle = NULL;
		m_threadState = ThreadState::THREADSTATE_UNSTART;
		return true;
	}
	return false;
}
/***************************************************************************
* 函数名称：   Suspend
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/08      饶智博        添加
***************************************************************************/
bool Thread::Suspend()							 // 挂起线程
{
	if (NULL == m_handle)
		return false;

	::SuspendThread(m_handle);
	m_threadState = ThreadState::THREADSTATE_SLEEPING;
	return true;
}

/***************************************************************************
* 函数名称：   Run
* 摘　　要：   
* 全局影响：   virtual protected 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/08      饶智博        添加
***************************************************************************/
void Thread::Run()
{
	if (NULL == m_handle)
		return;

	// 说明有任务,执行任务
	if (NULL != m_task)
	{
		m_threadState = ThreadState::THREADSTATE_BUSY;
		(*m_task)((void*)(&m_threadID));
	}

	m_threadState = ThreadState::THREADSTATE_TERMINATE;
}

/***************************************************************************
* 函数名称：   CreateThread
* 摘　　要：   
* 全局影响：   protected 
* 参　　数：   [in]  bool bSuspend
* 返回值　：   HANDLE
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/08      饶智博        添加
***************************************************************************/
HANDLE Thread::CreateThread(bool bSuspend)
{
	// 一个类中只能有一个线程
	if (NULL != m_handle)
		return m_handle;

	if (bSuspend)
	{
		m_handle = (HANDLE)_beginthreadex(NULL, 0, StaticThreadFunc, this, CREATE_SUSPENDED, &m_threadID);
		m_threadState = ThreadState::THREADSTATE_SLEEPING;
	}
	else
	{
		m_handle = (HANDLE)_beginthreadex(NULL, 0, StaticThreadFunc, this, 0, &m_threadID);
		m_threadState = ThreadState::THREADSTATE_AWAITING;
	}

	if (NULL == m_handle)
		assert(FALSE);

	return m_handle;
}

/***************************************************************************
* 函数名称：   StaticThreadFunc
* 摘　　要：   
* 全局影响：   private static 
* 参　　数：   [in]  void * arg
* 返回值　：   unsigned int
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/09      饶智博        添加
***************************************************************************/
unsigned int Thread::StaticThreadFunc(void * arg)
{
	Thread * pThread = (Thread *)arg;
	pThread->Run();
	return 0;
}


// 线程池
// 静态成员变量的初始化
ThreadManage* ThreadManage::m_instance = NULL;

/***************************************************************************
* 函数名称：   ThreadManage
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/06      饶智博        添加
***************************************************************************/
ThreadManage::ThreadManage(int minThreadNum, int maxThreadNum)
	:m_iTheadNum(0),
	m_iMaxThreadNum(maxThreadNum),
	m_iMinThreadNum(minThreadNum),
	m_bRun(false)
{
	// 先都清理掉

	if (m_iMinThreadNum > m_iMaxThreadNum || m_iMaxThreadNum <= 0 || m_iMinThreadNum <= 0)
	{
		assert(FALSE);
		return;
	}

	m_tasks.clear();
	m_threadPool.clear();
	InitializeCriticalSection(&m_csTasksLock);
	InitializeCriticalSection(&m_csThreadPoolLock);
	
	ST_THD_INFO temThreadInfo;
	for (int i = 0; i < m_iMinThreadNum; i++)
	{
		ZeroMemory(&temThreadInfo, sizeof(ST_THD_INFO));
		Worker* pWorker = new Worker(this, temThreadInfo);
		if (NULL == pWorker)
			return;

		EnterCriticalSection(&m_csThreadPoolLock);
		m_threadPool.push_back(temThreadInfo);
		LeaveCriticalSection(&m_csThreadPoolLock);

		pWorker->Start();
	}
	m_bRun = true;
}



/***************************************************************************
* 函数名称：   ~ThreadManage
* 摘　　要：   
* 全局影响：   virtual public 
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/06      饶智博        添加
***************************************************************************/
ThreadManage::~ThreadManage()
{

}

ThreadManage::Worker::Worker(ThreadManage* parent, ST_THD_INFO& threadInfo)
	:Thread(),
	 m_parent(parent)
{
	threadInfo.hStart = CreateEvent(NULL, TRUE, FALSE, NULL);
	threadInfo.hStart = CreateEvent(NULL, TRUE, FALSE, NULL);
	threadInfo.hStart = CreateEvent(NULL, TRUE, FALSE, NULL);
	threadInfo.hHasTask = CreateEvent(NULL, TRUE, FALSE, NULL);
	threadInfo.worker = (void*)this;
	memcpy(&m_stThdInfo, &threadInfo, sizeof(ST_THD_INFO));
}

/***************************************************************************
* 函数名称：   ~Worker
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/08      饶智博        添加
***************************************************************************/
ThreadManage::Worker::~Worker()
{
	if (NULL != m_handle)
		Exit();
}

void ThreadManage::Worker::Exit()
{
	// 说明可以停止了
	SetEvent(m_stThdInfo.hCanExit);
	SetEvent(m_stThdInfo.hStart);

	// 看是否收到线程停止的消息
	WaitForSingleObject(m_stThdInfo.hHasExit, INFINITE);

	// 关闭事件句柄
	CloseHandle(m_stThdInfo.hStart);
	CloseHandle(m_stThdInfo.hCanExit);
	CloseHandle(m_stThdInfo.hHasExit);
	CloseHandle(m_stThdInfo.hHasTask);

	// 关闭线程句柄
	CloseHandle(m_handle);
	m_handle = NULL;
}
/***************************************************************************
* 函数名称：   Run
* 摘　　要：   
* 全局影响：   protected 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/08      饶智博        添加
***************************************************************************/
void ThreadManage::Worker::Run()
{
	while (true)
	{
		// 稍息
		Sleep(10);
		
		// 等待执行时间，当有任务需要我去执行的时候在去执行
		WaitForSingleObject(m_stThdInfo.hStart, INFINITE);
		// 说明现在需要执行任务，修改当前数据状态
		m_threadState = ThreadState::THREADSTATE_BUSY;
		
		// 是否可以退出
		if (::WaitForSingleObject(m_stThdInfo.hCanExit, 0) == WAIT_OBJECT_0)
			break;

		// 取出任务
		Task task = NULL;
		task = m_parent->GetTask();

		// 执行任务
		if (NULL != task)
		{
			SetEvent(m_stThdInfo.hHasTask);
			(*task)((void*)&m_threadID);
		}
		// 暂停线程
		else
		{
			ResetEvent(m_stThdInfo.hStart);
			// 说明现在可以接任务
			m_threadState = ThreadState::THREADSTATE_AWAITING;
		}
		
	}

	SetEvent(m_stThdInfo.hHasExit);
}

/***************************************************************************
* 函数名称：   GetTask
* 摘　　要：   
* 全局影响：   private 
* 返回值　：   Task
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/08      饶智博        添加
***************************************************************************/
Task ThreadManage::GetTask()
{
	Task  task = NULL;
	EnterCriticalSection(&m_csTasksLock);
	if (!m_tasks.empty())
	{
		task = m_tasks.front();
		m_tasks.pop_front();
	}
	LeaveCriticalSection(&m_csTasksLock);
	return task;
}



/***************************************************************************
* 函数名称：   CreateInstance
* 摘　　要：   
* 全局影响：   public static 
* 返回值　：   ThreadManage*
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/06      饶智博        添加
***************************************************************************/
ThreadManage* ThreadManage::CreateInstance(int minThreadNum, int maxThreadNum)
{
	if (NULL != m_instance)
		return m_instance;

	m_instance = new ThreadManage(minThreadNum, maxThreadNum);
	return m_instance;
}

/***************************************************************************
* 函数名称：   DeleteInstance
* 摘　　要：   
* 全局影响：   public static 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/06      饶智博        添加
***************************************************************************/
void ThreadManage::DeleteInstance()
{
	if (NULL != m_instance)
		delete m_instance;
	
	m_instance = NULL;
}

bool ThreadManage::Execute(Task task)
{
	if (!m_bRun || NULL == task)
		return false;
	// 将任务插入队列即可
	EnterCriticalSection(&m_csTasksLock);
	m_tasks.push_back(task);
	LeaveCriticalSection(&m_csTasksLock);

	// 已经有的线程是否可以完成任务
	Worker* pWorker = NULL;
	ThreadPoolItr rit;
	for (rit = m_threadPool.begin(); rit != m_threadPool.end(); rit++)
	{
		pWorker = ((Worker*)(rit->worker));
		// 如果当前状态是休息那么就去执行任务
		if (pWorker->CurrentStatus()==ThreadState::THREADSTATE_AWAITING)
		{
			SetEvent(rit->hStart);
			return true;
		}
	}

	// 需要新建线程了
	if (m_threadPool.size() < m_iMaxThreadNum)
	{
		ST_THD_INFO temThreadInfo;
		pWorker = new Worker(this, temThreadInfo);
		if (NULL == pWorker)
			return false;

		EnterCriticalSection(&m_csThreadPoolLock);
		m_threadPool.push_back(temThreadInfo);
		LeaveCriticalSection(&m_csThreadPoolLock);
		
		pWorker->Start();
		SetEvent(temThreadInfo.hStart);
		return true;
	}

	return false;
}