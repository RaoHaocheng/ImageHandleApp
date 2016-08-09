#include "ThreadManage.h"
#include <process.h>
#include <assert.h>

// �߳�
// FIXME::Ŀǰ�߳��е�״̬���������⣬��Ҫ��ʱ�ĵ����������̳߳��еĴ����������
/***************************************************************************
* �������ƣ�   Thread
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/08      ���ǲ�        ���
***************************************************************************/
Thread::Thread()
{
	Thread(NULL);
}


/***************************************************************************
* �������ƣ�   Thread
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  Task * task
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/08      ���ǲ�        ���
***************************************************************************/
Thread::Thread(Task task)
	:m_task(task),
	m_handle(NULL),
	m_threadState(THREADSTATE_UNSTART),
	m_threadID(-1)
{
}


/***************************************************************************
* �������ƣ�   ~Thread
* ժ����Ҫ��   
* ȫ��Ӱ�죺   virtual public 
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/08      ���ǲ�        ���
***************************************************************************/
Thread::~Thread()
{

}


/***************************************************************************
* �������ƣ�   SetNewTask
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  Task * task
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/08      ���ǲ�        ���
***************************************************************************/
bool Thread::SetNewTask(Task task)
{
	if (NULL != m_handle)
		return false;

	m_task = task;
	return true;
}

/***************************************************************************
* �������ƣ�   ClearTask
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/08      ���ǲ�        ���
***************************************************************************/
bool Thread::ClearTask()
{
	if (NULL != m_handle)
		return false;

	m_task = NULL;
	return true;
}

/***************************************************************************
* �������ƣ�   Start
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  bool bSuspend
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/08      ���ǲ�        ���
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
* �������ƣ�   Pause
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  int timeout
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/08      ���ǲ�        ���
***************************************************************************/
bool Thread::Pause(int timeout)// ��ͣ�߳�
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
* �������ƣ�   Resume
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/08      ���ǲ�        ���
***************************************************************************/
bool Thread::Resume()							 // �ָ��߳�
{
	if (NULL == m_handle)
		return false;

	::ResumeThread(m_handle);
	return true;
}
/***************************************************************************
* �������ƣ�   Exit
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  unsigned long ExitCode
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/08      ���ǲ�        ���
***************************************************************************/
bool Thread::Exit(unsigned long ExitCode)      // �˳��߳�
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
* �������ƣ�   Suspend
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/08      ���ǲ�        ���
***************************************************************************/
bool Thread::Suspend()							 // �����߳�
{
	if (NULL == m_handle)
		return false;

	::SuspendThread(m_handle);
	m_threadState = ThreadState::THREADSTATE_SLEEPING;
	return true;
}

/***************************************************************************
* �������ƣ�   Run
* ժ����Ҫ��   
* ȫ��Ӱ�죺   virtual protected 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/08      ���ǲ�        ���
***************************************************************************/
void Thread::Run()
{
	if (NULL == m_handle)
		return;

	// ˵��������,ִ������
	if (NULL != m_task)
	{
		m_threadState = ThreadState::THREADSTATE_BUSY;
		(*m_task)((void*)(&m_threadID));
	}

	m_threadState = ThreadState::THREADSTATE_TERMINATE;
}

/***************************************************************************
* �������ƣ�   CreateThread
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  bool bSuspend
* ����ֵ����   HANDLE
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/08      ���ǲ�        ���
***************************************************************************/
HANDLE Thread::CreateThread(bool bSuspend)
{
	// һ������ֻ����һ���߳�
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
* �������ƣ�   StaticThreadFunc
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private static 
* �Ρ�������   [in]  void * arg
* ����ֵ����   unsigned int
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/09      ���ǲ�        ���
***************************************************************************/
unsigned int Thread::StaticThreadFunc(void * arg)
{
	Thread * pThread = (Thread *)arg;
	pThread->Run();
	return 0;
}


// �̳߳�
// ��̬��Ա�����ĳ�ʼ��
ThreadManage* ThreadManage::m_instance = NULL;

/***************************************************************************
* �������ƣ�   ThreadManage
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/06      ���ǲ�        ���
***************************************************************************/
ThreadManage::ThreadManage(int minThreadNum, int maxThreadNum)
	:m_iTheadNum(0),
	m_iMaxThreadNum(maxThreadNum),
	m_iMinThreadNum(minThreadNum),
	m_bRun(false)
{
	// �ȶ������

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
* �������ƣ�   ~ThreadManage
* ժ����Ҫ��   
* ȫ��Ӱ�죺   virtual public 
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/06      ���ǲ�        ���
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
* �������ƣ�   ~Worker
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/08      ���ǲ�        ���
***************************************************************************/
ThreadManage::Worker::~Worker()
{
	if (NULL != m_handle)
		Exit();
}

void ThreadManage::Worker::Exit()
{
	// ˵������ֹͣ��
	SetEvent(m_stThdInfo.hCanExit);
	SetEvent(m_stThdInfo.hStart);

	// ���Ƿ��յ��߳�ֹͣ����Ϣ
	WaitForSingleObject(m_stThdInfo.hHasExit, INFINITE);

	// �ر��¼����
	CloseHandle(m_stThdInfo.hStart);
	CloseHandle(m_stThdInfo.hCanExit);
	CloseHandle(m_stThdInfo.hHasExit);
	CloseHandle(m_stThdInfo.hHasTask);

	// �ر��߳̾��
	CloseHandle(m_handle);
	m_handle = NULL;
}
/***************************************************************************
* �������ƣ�   Run
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/08      ���ǲ�        ���
***************************************************************************/
void ThreadManage::Worker::Run()
{
	while (true)
	{
		// ��Ϣ
		Sleep(10);
		
		// �ȴ�ִ��ʱ�䣬����������Ҫ��ȥִ�е�ʱ����ȥִ��
		WaitForSingleObject(m_stThdInfo.hStart, INFINITE);
		// ˵��������Ҫִ�������޸ĵ�ǰ����״̬
		m_threadState = ThreadState::THREADSTATE_BUSY;
		
		// �Ƿ�����˳�
		if (::WaitForSingleObject(m_stThdInfo.hCanExit, 0) == WAIT_OBJECT_0)
			break;

		// ȡ������
		Task task = NULL;
		task = m_parent->GetTask();

		// ִ������
		if (NULL != task)
		{
			SetEvent(m_stThdInfo.hHasTask);
			(*task)((void*)&m_threadID);
		}
		// ��ͣ�߳�
		else
		{
			ResetEvent(m_stThdInfo.hStart);
			// ˵�����ڿ��Խ�����
			m_threadState = ThreadState::THREADSTATE_AWAITING;
		}
		
	}

	SetEvent(m_stThdInfo.hHasExit);
}

/***************************************************************************
* �������ƣ�   GetTask
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private 
* ����ֵ����   Task
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/08      ���ǲ�        ���
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
* �������ƣ�   CreateInstance
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public static 
* ����ֵ����   ThreadManage*
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/06      ���ǲ�        ���
***************************************************************************/
ThreadManage* ThreadManage::CreateInstance(int minThreadNum, int maxThreadNum)
{
	if (NULL != m_instance)
		return m_instance;

	m_instance = new ThreadManage(minThreadNum, maxThreadNum);
	return m_instance;
}

/***************************************************************************
* �������ƣ�   DeleteInstance
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public static 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/06      ���ǲ�        ���
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
	// �����������м���
	EnterCriticalSection(&m_csTasksLock);
	m_tasks.push_back(task);
	LeaveCriticalSection(&m_csTasksLock);

	// �Ѿ��е��߳��Ƿ�����������
	Worker* pWorker = NULL;
	ThreadPoolItr rit;
	for (rit = m_threadPool.begin(); rit != m_threadPool.end(); rit++)
	{
		pWorker = ((Worker*)(rit->worker));
		// �����ǰ״̬����Ϣ��ô��ȥִ������
		if (pWorker->CurrentStatus()==ThreadState::THREADSTATE_AWAITING)
		{
			SetEvent(rit->hStart);
			return true;
		}
	}

	// ��Ҫ�½��߳���
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