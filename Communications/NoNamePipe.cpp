#include "NoNamePipe.h"


/***************************************************************************
* 函数名称：   NoNamePipe
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  ProcessType type
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/21      饶智博        添加
***************************************************************************/
NoNamePipe::NoNamePipe(ProcessType type):
	m_hPipeRead(NULL),
	m_hPipeWrite(NULL),
	m_type(type)
{
}

/***************************************************************************
* 函数名称：   ~NoNamePipe
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/21      饶智博        添加
***************************************************************************/
NoNamePipe::~NoNamePipe()
{
	CloseHandle(m_hPipeWrite);
	CloseHandle(m_hPipeRead);
}

/***************************************************************************
* 函数名称：   StartProcess
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  char * szCmdLine
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/21      饶智博        添加
***************************************************************************/
bool NoNamePipe::StartProcess(char* szCmdLine)
{
	if (ParentProcess != m_type)
		return false;

	ZeroMemory(&m_piProcInfo, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&m_siStartInfo, sizeof(STARTUPINFO));
	m_siStartInfo.cb = sizeof(STARTUPINFO);
	m_siStartInfo.hStdError = m_hPipeWrite;
	m_siStartInfo.hStdOutput = m_hPipeWrite;
	m_siStartInfo.hStdInput = m_hPipeRead;
	m_siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	bool bSuccess = CreateProcess(NULL,
		szCmdLine,	   // command line 
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		TRUE,          // handles are inherited 
		0,             // creation flags 
		NULL,          // use parent's environment 
		NULL,          // use parent's current directory 
		&m_siStartInfo,// STARTUPINFO pointer 
		&m_piProcInfo);// receives PROCESS_INFORMATION 

	if (!bSuccess)
		return false;
	else
	{
		CloseHandle(m_piProcInfo.hProcess);
		CloseHandle(m_piProcInfo.hThread);
		return true;
	}
}

/***************************************************************************
* 函数名称：   WriteToPipe
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const char * buf
* 参　　数：   [in]  size_t & len
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/21      饶智博        添加
***************************************************************************/
bool NoNamePipe::WriteToPipe(const char* buf, size_t& len)
{
	DWORD dwWritten;
	bool bSuccess = WriteFile(m_hPipeWrite, buf, len, &dwWritten, NULL);
	len = dwWritten;
	return bSuccess;
}

/***************************************************************************
* 函数名称：   InitNoNamePipe
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/21      饶智博        添加
***************************************************************************/
bool NoNamePipe::InitNoNamePipe()
{
	bool bRet;
	switch (m_type)
	{
	case ParentProcess:
		bRet = CreateNoNamePipe();
		break;
	case ChildProcess:
		bRet = GetNoNamePipe();
		break;
	default:
		break;
	}

	return bRet;
}

/***************************************************************************
* 函数名称：   CreateNoNamePipe
* 摘　　要：   
* 全局影响：   protected 
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/21      饶智博        添加
***************************************************************************/
bool NoNamePipe::CreateNoNamePipe()
{
	m_saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	m_saAttr.bInheritHandle = true;
	m_saAttr.lpSecurityDescriptor = NULL;

	// 创建父进程管道
	if (!CreatePipe(&m_hPipeRead, &m_hPipeWrite, &m_saAttr, 0))
		return false;

	if (!SetHandleInformation(m_hPipeRead, HANDLE_FLAG_INHERIT, 0)
		&& !SetHandleInformation(m_hPipeWrite, HANDLE_FLAG_INHERIT, 0))
		return false;
}

/***************************************************************************
* 函数名称：   GetNoNamePipe
* 摘　　要：   
* 全局影响：   protected 
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/21      饶智博        添加
***************************************************************************/
bool NoNamePipe::GetNoNamePipe()
{
	m_hPipeRead = GetStdHandle(STD_OUTPUT_HANDLE);
	m_hPipeWrite = GetStdHandle(STD_INPUT_HANDLE);
	if ((m_hPipeRead == INVALID_HANDLE_VALUE) ||
		(m_hPipeWrite == INVALID_HANDLE_VALUE))
		return false;
	else
		return true;
}

/***************************************************************************
* 函数名称：   ReadFromPipe
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  char * buf
* 参　　数：   [in]  size_t & len
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/21      饶智博        添加
***************************************************************************/
bool NoNamePipe::ReadFromPipe(char* buf, size_t& len)
{
	DWORD dwRead;
	bool bSuccess = ReadFile(m_hPipeRead, buf, len, &dwRead, NULL);
	len = dwRead;
	return bSuccess;
}