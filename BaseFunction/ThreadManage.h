#ifndef THREADMANGE_H
#define THREADMANGE_H
// �̳߳�
#include "define.h"


// �̵߳�ǰ��״̬
enum ThreadState
{
	THREADSTATE_UNSTART = -1,  // δ��ʼ
	THREADSTATE_TERMINATE = 0, // ��ֹ
	THREADSTATE_PAUSED = 1,    // ��ͣ
	THREADSTATE_SLEEPING = 2,  // ˯��
	THREADSTATE_BUSY = 3,      // æµ
	THREADSTATE_AWAITING = 4   // �Ⱥ�
};

// ������
// ���Ƿ���ʹ�������ಢ�����㣬���ǻ���ϣ��ʹ�ú���ָ����ɸù���
// class DLLS_PORT Task
// {
// public:
// 	virtual ~Task() {};
// 	virtual void Run() = 0;
// };

// ����ĺ�������
typedef int (*Task)(void*);

// �߳���
class DLLS_PORT Thread
{
public:
	Thread();
	Thread(Task task);       // ������Ҫд��Ĺ���
	virtual ~Thread();        // ��������

	// �Ա���Ĳ���
	bool SetNewTask(Task task); // �����µ�����,ֻ�Ի�δ��ʼ��������Ч
	bool ClearTask();            // �����ǰ������

	bool Start(bool bSuspend = false);       // �����߳�
	bool Pause(int timeout = -1);		 	 // ��ͣ�߳�
	bool Resume();							 // �ָ��߳�
	bool Exit(unsigned long ExitCode);		 // �˳��߳�
	bool Suspend();							 // �����߳�

	inline ThreadState CurrentStatus() { return m_threadState; } //�����ص�ǰ�Ĺ���״̬
	
protected:
	HANDLE CreateThread(bool);
	virtual void Run();                     // ִ�к���
	HANDLE m_handle;					    // ��ǰ�̵߳ľ��
	unsigned int m_threadID;                // �̱߳�
	ThreadState m_threadState;              // ��ǰ���߳�״̬

private:
	static unsigned int WINAPI StaticThreadFunc(void * arg);   // ���������ڲ����õ��̺߳���

private:
	Task  m_task;              // ���߳���Ҫִ�е������
};

// �̳߳���
// typedef struct DLLS_PORT ThreadID
// {
// 	Thread thread;
// } ST_THREAD_ID, *PST_THREAD_ID;                                          // ��Ӧ���̺߳Ͷ�Ӧ��id����һ��
// ʹ��ģ����ĵ�������
//template class DLLS_PORT std::vector<ST_THREAD_ID>;
typedef struct _ST_THD_INFO
{
	void* worker;               // ��Ӧ�̵߳�ָ��
	HANDLE hStart;				// �����¼�
	HANDLE hHasTask;			// �Ѿ����ܵ�����
	HANDLE hCanExit;			// �����˳��¼�
	HANDLE hHasExit;			// �Ѿ��˳��¼�
}ST_THD_INFO, *PST_THD_INFO;
template class DLLS_PORT std::vector<ST_THD_INFO>;
template class DLLS_PORT std::list<Task>;

class DLLS_PORT ThreadManage
{
// Ϊ��ֻ�ø�����һ��������ֻ������һ��ʵ��
public:
	static ThreadManage* CreateInstance(int minThreadNum,int maxThreadNum);// �ⲿʹ�øþ�̬������������ʵ��
	static void DeleteInstance();						   // ���ٸ���ʵ��
	bool Execute(Task task);                               // ʹ��id�������Ӧ���߳�

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
	friend Worker;                                           // ��Ҫ���ڴ������Ե��

	typedef std::list<Task> Tasks;							 // �����б�
	typedef std::vector<ST_THD_INFO> ThreadPool;		     // ���ڴ���߳�
	typedef ThreadPool::iterator  ThreadPoolItr;             // ���ڴ���߳�

	int m_iTheadNum;                            // �̵߳�ǰ
	static ThreadManage* m_instance;            // ָ��ǰ��ʵ����ָ��
	int m_iMinThreadNum;                        // ��С�߳���
	int m_iMaxThreadNum;                        // ����߳���

	Tasks m_tasks;                              // �ҵ������б�
	ThreadPool m_threadPool;                    // �ҵ��̳߳�
	CRITICAL_SECTION m_csTasksLock;             // �����б���ٽ���
	CRITICAL_SECTION m_csThreadPoolLock;        // �̳߳ص��ٽ���

	bool m_bRun;                                // ��ǰ�̳߳��Ƿ����
};

#endif // !BASEFUNCTION_H